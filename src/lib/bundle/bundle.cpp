#include "fourdst/plugin/bundle/bundle.h"
#include "fourdst/plugin/manager/plugin_manager.h"
#include "fourdst/plugin/factory/plugin_factory.h"

#include "fourdst/crypt/public_key.h"
#include "fourdst/crypt/crypt_verification.h"
#include "fourdst/crypt/openSSL_utils.h"

#include "mz.h"
#include "mz_zip.h"
#include "mz_strm.h"
#include "mz_strm_os.h" // For file stream operations

#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <stdexcept>

#include <sys/utsname.h>
#if defined(__linux__)
    #include <gnu/libc-version.h>
#elif defined(__APPLE__)
    #include <sys/sysctl.h>
#endif

#include <unistd.h>
#include <pwd.h>

#include "mz_zip_rw.h"
#include <expected>

namespace {
    std::filesystem::path get_home_directory() {
        const char* homeEnv = getenv("HOME");
        if (homeEnv != nullptr && homeEnv[0] != '\0') {
            return std::filesystem::path(homeEnv);
        }

        const uid_t uid = getuid();
        struct passwd* pw = getpwuid(uid);
        if (pw != nullptr && pw->pw_dir != nullptr) {
            return std::filesystem::path(pw->pw_dir);
        }

        throw std::runtime_error("Unable to determine home directory (are you running on a POSIX compliant system?)!");
    }
    void check_mz_error(int32_t err, const std::string& msg) {
        if (err != MZ_OK) {
            std::ostringstream os;
            os << "Minizip Error: " << msg << " (code: " << err << ")";
            const std::string error_message = os.str();
            throw std::runtime_error(error_message);
        }
    }

    void unzip_archive(const std::filesystem::path& archive_path, const std::filesystem::path& output_dir) {
        namespace fs = std::filesystem;
        fs::create_directories(output_dir);


        void* reader_handle = mz_zip_reader_create();
        check_mz_error(reader_handle ? MZ_OK : MZ_PARAM_ERROR, "Failed to create zip reader");

        check_mz_error(mz_zip_reader_open_file(reader_handle, archive_path.c_str()), "Failed to open archive for reading");

        int32_t err = mz_zip_reader_goto_first_entry(reader_handle);
        if (err != MZ_OK && err != MZ_END_OF_LIST) {
            check_mz_error(err, "Failed to go to first entry");
        }

        while (err == MZ_OK) {
            mz_zip_file* file_info = nullptr;
            check_mz_error(mz_zip_reader_entry_get_info(reader_handle, &file_info), "Failed to get entry info");

            fs::path dest_path = output_dir / fs::path(file_info->filename).lexically_normal();

            if (mz_zip_entry_is_dir(file_info) == MZ_OK) {
                fs::create_directories(dest_path);
            } else {
                fs::create_directories(dest_path.parent_path());

                check_mz_error(mz_zip_reader_entry_open(reader_handle), "Failed to open entry for reading");

                std::ofstream out_file(dest_path, std::ios::binary);
                if (!out_file.is_open()) {
                    throw std::runtime_error("Failed to open output file: " + dest_path.string());
                }

                int32_t bytes_read = 0;
                std::vector<char> buffer(16384); // 16KB buffer
                do {
                    bytes_read = mz_zip_reader_entry_read(reader_handle, buffer.data(), buffer.size());
                    if (bytes_read < 0) {
                         check_mz_error(bytes_read, "Failed to read entry data");
                    }
                    if (bytes_read > 0) {
                        out_file.write(buffer.data(), bytes_read);
                    }
                } while (bytes_read > 0);

                out_file.close();

                check_mz_error(mz_zip_reader_entry_close(reader_handle), "Failed to close entry");
            }

            err = mz_zip_reader_goto_next_entry(reader_handle);
            if (err != MZ_OK && err != MZ_END_OF_LIST) {
                check_mz_error(err, "Failed to go to next entry");
            }
        }

        mz_zip_reader_close(reader_handle);
        mz_zip_reader_delete(&reader_handle);
    }

    std::vector<unsigned char> file_to_vector(const std::filesystem::path& filepath) {
        std::ifstream ifs(filepath, std::ios::binary);
        if (!ifs) {
            throw std::runtime_error("Failed to open file: " + filepath.string());
        }
        std::vector<unsigned char> data((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        ifs.close();
        return data;
    }

    std::vector<unsigned char> hex_string_to_bytes(const std::string &hex) {
        if (hex.size() % 2 != 0) {
            throw std::runtime_error("Hex string length must be even");
        }

        std::vector<unsigned char> bytes;
        bytes.reserve(hex.size() / 2);

        auto hex_char_to_val = [](char c) -> unsigned char {
            if (std::isdigit(static_cast<unsigned char>(c))) return c - '0';
            if (std::isxdigit(static_cast<unsigned char>(c))) return std::tolower(c) - 'a' + 10;
            throw std::runtime_error("Invalid hex character");
        };

        for (std::size_t i = 0; i < hex.size(); i += 2) {
            unsigned char high = hex_char_to_val(hex[i]);
            unsigned char low  = hex_char_to_val(hex[i + 1]);
            bytes.push_back((high << 4) | low);
        }

        return bytes;
    }

    std::string reconstruct_and_verify(
    const std::filesystem::path& temp_dir,
    const YAML::Node& manifest
    ) {
        std::map<std::string, std::string> checksum_map;
        std::vector<YAML::Node> all_files;

        // 1. Gather all file entries from the manifest (This part is correct)
        for (const auto& plugin_node : manifest["bundlePlugins"]) {
            const auto& plugin_data = plugin_node.second;
            if (plugin_data["sdist"] && plugin_data["sdist"]["path"]) {
                all_files.push_back(plugin_data["sdist"]);
            }
            if (plugin_data["binaries"]) {
                for (const auto& binary_node : plugin_data["binaries"]) {
                    all_files.push_back(binary_node);
                }
            }
        }

        // 2. Calculate checksums for files on disk (This part is correct)
        for (const auto& file_node : all_files) {
            auto path_str = file_node["path"].as<std::string>();
            std::filesystem::path file_path = temp_dir / path_str;

            if (!std::filesystem::exists(file_path)) {
                throw std::runtime_error("File listed in manifest is missing: " + path_str);
            }

            std::string actual_checksum = "sha256:" + fourdst::crypt::utils::calculate_sha256(file_path);
            checksum_map[path_str] = actual_checksum;
        }

        // 3. Build the canonical string using a robust join method
        std::stringstream canonical_stream;
        for (auto it = checksum_map.begin(); it != checksum_map.end(); ++it) {
            if (it != checksum_map.begin()) {
                // Add the newline separator BEFORE each element except the first one
                canonical_stream << "\n";
            }
            canonical_stream << it->first << ":" << it->second;
        }

        return canonical_stream.str();
    }

    bool is_valid_public_key_pem(const std::filesystem::path& filePath)
    {
        if (!std::filesystem::exists(filePath) || !std::filesystem::is_regular_file(filePath)) {
            return false;
        }

        std::ifstream fileStream(filePath, std::ios::in);
        if (!fileStream) {
            return false;
        }

        std::string line;
        std::string firstLine;
        std::string lastLine;

        while (std::getline(fileStream, line)) {
            if (line.empty()) {
                continue;
            }
            if (firstLine.empty()) {
                firstLine = line;
            }
            lastLine = line;
        }

        static const std::string beginMarker = "-----BEGIN PUBLIC KEY-----";
        static const std::string endMarker   = "-----END PUBLIC KEY-----";

        return (firstLine == beginMarker) && (lastLine == endMarker);
    }


    enum class Key_Expectation {
        TRUSTED_KEY_DIR_DOES_NOT_EXIST,
    };

    std::expected<std::vector<fourdst::crypt::PublicKey>, Key_Expectation> get_host_trusted_keys() {
        std::filesystem::path keys_dir = get_home_directory() / ".config/fourdst/keys";
        if (!std::filesystem::exists(keys_dir)) {
            return std::unexpected(Key_Expectation::TRUSTED_KEY_DIR_DOES_NOT_EXIST);
        }
        std::vector<fourdst::crypt::PublicKey> trusted_keys;
        for (auto const& entry : std::filesystem::recursive_directory_iterator(keys_dir)) {
            if (entry.is_regular_file()) {
                if (is_valid_public_key_pem(entry)) {
                    std::filesystem::path path = entry.path();
                    trusted_keys.emplace_back(fourdst::crypt::PublicKey(path));
                }
            }
        }
        return trusted_keys;
    }

    struct ABISignature {
        std::string compiler;
        std::string library;
        std::vector<int> version_parts;
        std::string abi_type;

        void print() const {
            std::cout << "Compiler: " << compiler << ", Library: " << library
                      << ", ABI: " << abi_type << ", Version: ";
            for (size_t i = 0; i < version_parts.size(); ++i) {
                std::cout << version_parts[i] << (i < version_parts.size() - 1 ? "." : "");
            }
            std::cout << std::endl;
        }
    };

    std::optional<ABISignature> parse_abi_signature(const std::string& sig_str) {
        ABISignature sig;
        std::vector<std::string> parts;
        std::stringstream ss(sig_str);
        std::string item;

        while (std::getline(ss, item, '-')) {
            parts.push_back(item);
        }

        if (parts.size() != 4) {
            return std::nullopt;
        }

        sig.compiler = parts[0];
        sig.library = parts[1];
        sig.abi_type = parts[3];

        // Parse the version string (parts[2])
        std::stringstream version_ss(parts[2]);
        std::string version_part_str;
        while (std::getline(version_ss, version_part_str, '.')) {
            try {
                sig.version_parts.push_back(std::stoi(version_part_str));
            } catch (const std::invalid_argument& e) {
                return std::nullopt;
            } catch (const std::out_of_range& e) {
                return std::nullopt;
            }
        }

        return sig;
    }
    bool is_abi_compatible(const ABISignature& local, const ABISignature& required) {
        if (local.compiler != required.compiler ||
            local.library  != required.library  ||
            local.abi_type != required.abi_type) {
            return false;
            }

        const auto& local_v = local.version_parts;
        const auto& req_v = required.version_parts;
        const size_t min_len = std::min(local_v.size(), req_v.size());

        for (size_t i = 0; i < min_len; ++i) {
            if (local_v[i] > req_v[i]) {
                return true; // e.g., local is 3.5, required is 3.4
            }
            if (local_v[i] < req_v[i]) {
                return false; // e.g., local is 3.4, required is 3.5
            }
        }

        return local_v.size() >= req_v.size();
    }

}

namespace fourdst::plugin::bundle {
    bool PluginBundle::verify_bundle() {
        m_trusted = false;
        m_signed = false;
        auto signatureSection = m_bundleManifest["bundleSignature"];
        if (!signatureSection) {
            return false; // No signature section found
        }
        else {
            auto signatureHexString = signatureSection["signature"].as<std::string>("");
            if (signatureHexString.empty()) {
                throw std::runtime_error("Bundle signature is empty in the manifest even though there is a signature section. This is likely a malformed bundle manifest.");
            }
            m_signed = true;
            m_bundleSignature = hex_string_to_bytes(signatureHexString);
            if (signatureSection["keyFingerprint"]) {
                m_bundleAuthorKeyFingerprint = signatureSection["keyFingerprint"].as<std::string>();
            } else {
                m_signed = false;
                throw std::runtime_error("Bundle author key fingerprint is missing in the manifest with signature!");
            }

            if (m_bundleAuthorKeyFingerprint && !m_bundleSignature->empty()) {
                try {
                    std::string data_to_verify_str = reconstruct_and_verify(
                        m_temporaryDirectory.get_path(),
                        m_bundleManifest
                    );
                    const std::vector<unsigned char> data_to_verify_vec(data_to_verify_str.begin(), data_to_verify_str.end());

                    auto trusted_keys_expectation = get_host_trusted_keys();
                    if (!trusted_keys_expectation) {
                        throw std::runtime_error("Trusted keys directory does not exist or no trusted keys found.");
                    }

                    const std::vector<crypt::PublicKey> trusted_keys = std::move(trusted_keys_expectation.value());
                    size_t current_index = 0;
                    size_t trusted_key_index = -1;
                    for (const auto& trusted_key : trusted_keys) {
                        if (trusted_key.get_fingerprint() == m_bundleAuthorKeyFingerprint.value()) {
                            trusted_key_index = current_index;
                        }
                        current_index++;
                    }

                    if (trusted_key_index == static_cast<size_t>(-1)) {
                        throw std::runtime_error("No trusted key found matching the bundle author fingerprint: " + m_bundleAuthorKeyFingerprint.value());
                    }


                    if (!trusted_keys[trusted_key_index].get()) {
                        throw std::runtime_error("Key object is invalid after construction.");
                    }
                    if (fourdst::crypt::verify_signature(trusted_keys[trusted_key_index], data_to_verify_vec, *m_bundleSignature)) {
                        m_trusted = true; // Verification successful!
                    } else {
                        m_trusted = false;
                        m_signed = false;
                    }

                } catch (const std::exception& e) {
                    std::cerr << "An error occurred during signature verification: " << e.what() << std::endl;
                    m_trusted = false;
                }
            }
        }
        return m_trusted && m_signed;
    }

    void PluginBundle::build_host_metadata() {
        m_hostABISignature = getHostABISignature();
        m_hostArchitecture = getHostArchitecture();
        m_hostOperatingSystem = getHostOperatingSystem();
        m_triplet = m_hostArchitecture + "-" + m_hostOperatingSystem;
    }

    std::vector<PluginPlatforms> PluginBundle::parse_manifest(const std::filesystem::path& manifestPath) {
        m_bundleManifest = YAML::LoadFile(manifestPath.string());

        m_bundleName = m_bundleManifest["bundleName"].as<std::string>();
        m_bundleVersion = m_bundleManifest["bundleVersion"].as<std::string>();
        m_bundleAuthor = m_bundleManifest["bundleAuthor"].as<std::string>();
        m_bundleComment = m_bundleManifest["bundleComment"].as<std::string>();
        m_bundledDatetime = m_bundleManifest["bundledOn"].as<std::string>();

        if (const bool trusted = verify_bundle(); !trusted) {
            throw std::runtime_error("Bundle verification failed or bundle is not trusted.");
        }

        if (!m_bundleManifest["bundlePlugins"]) {
            throw std::runtime_error("Bundle manifest does not contain 'bundlePlugins' section.");
        }

        std::vector<PluginPlatforms> bundledPlugins;
        size_t total_plugins_arch_independent = 0;
        for (const auto& plugin_node : m_bundleManifest["bundlePlugins"]) {
            bool counted_for_arch = false;
            const auto& plugin_data = plugin_node.second;
            const std::string plugin_name = plugin_node.first.as<std::string>();
            const auto& binaries_node = plugin_data["binaries"];
            if (!binaries_node) {
                throw std::runtime_error("Plugin entry missing 'binaries' section for plugin: " + plugin_name);
            }

            if (!binaries_node.IsSequence()) {
                throw std::runtime_error("Plugin entry 'binaries' section is not a sequence for plugin: " + plugin_name);
            }

            for (const auto& entry_node : binaries_node) {
                YAML::Node platform_node = entry_node["platform"];
                if (!platform_node || !platform_node.IsMap()) {
                    throw std::runtime_error("Plugin entry 'platform' section is missing or not a map for plugin: " + plugin_name);
                }

                if (!platform_node["triplet"]) {
                    throw std::runtime_error("Plugin entry 'platform' section is missing 'triplet' for plugin: " + plugin_name);
                }
                if (!platform_node["abi_signature"]) {
                    throw std::runtime_error("Plugin entry 'platform' section is missing 'abi_signature' for plugin: " + plugin_name);
                }
                if (!platform_node["arch"]) {
                    throw std::runtime_error("Plugin entry 'platform' section is missing 'arch' for plugin: " + plugin_name);
                }
                if (!entry_node["path"]) {
                    throw std::runtime_error("Plugin entry is missing 'path' for plugin: " + plugin_name);
                }

                const std::string triplet = platform_node["triplet"].as<std::string>();
                const std::string abi_signature = platform_node["abi_signature"].as<std::string>();
                const std::string architecture = platform_node["arch"].as<std::string>();
                const std::string path = entry_node["path"].as<std::string>();
                bundledPlugins.push_back(PluginPlatforms(plugin_name, triplet, abi_signature, architecture, path));
                if (!counted_for_arch) {
                    total_plugins_arch_independent++;
                    counted_for_arch = true;
                }
            }
        }

        auto HostABISignature = parse_abi_signature(m_hostABISignature);
        if (!HostABISignature) {
            throw std::runtime_error("Failed to parse host ABI signature: " + m_hostABISignature);
        }
        std::vector<PluginPlatforms> goodPlugins;
        for (const auto& plugin: bundledPlugins) {
            if (plugin.triplet != m_triplet) {
                continue; // Skip plugins that do not match the host triplet
            }

            auto pluginABISignature = parse_abi_signature(plugin.abiSignature);
            if (!pluginABISignature) {
                throw std::runtime_error("Failed to parse plugin ABI signature: " + plugin.abiSignature);
            }

            if (is_abi_compatible(HostABISignature.value(), pluginABISignature.value())) {
                goodPlugins.push_back(plugin);
            }
        }

        if (goodPlugins.size() != total_plugins_arch_independent) {
            switch (m_loadPolicy) {
                case PluginLoadPolicy::ALL_PLUGINS_ABI_COMPATIBLE:
                    throw std::runtime_error("Not all plugins are ABI compatible with the host system. Required: " + std::to_string(total_plugins_arch_independent) + ", Found: " + std::to_string(goodPlugins.size()));
                case PluginLoadPolicy::ANY_PLUGINS_ABI_COMPATIBLE:
                    if (goodPlugins.size() == 0) {
                        throw std::runtime_error("No plugins are ABI compatible with the host system.");
                    }
                    break;
            }
        }

        m_pluginNames.reserve(goodPlugins.size());
        for (const auto& plugin : goodPlugins) {
            m_pluginNames.push_back(plugin.name);
        }

        return goodPlugins;
    }

    PluginBundle::PluginBundle(const std::filesystem::path &filename, const PluginLoadPolicy policy) :
    m_loadPolicy(policy), m_pluginManager(manager::PluginManager::getInstance()) {
        if (!std::filesystem::exists(filename)) {
            throw std::runtime_error("Plugin bundle file does not exist: " + filename.string());
        }
        m_filepath = filename;

        unpackBundle(filename, m_temporaryDirectory);

        build_host_metadata();

        const std::filesystem::path manifestPath = m_temporaryDirectory.get_path() / "manifest.yaml";
        if (!std::filesystem::exists(manifestPath)) {
            throw std::runtime_error("Manifest file does not exist in the unpacked bundle: " + manifestPath.string());
        }

        m_trusted = false;
        m_signed = false;
        const std::vector<PluginPlatforms> good_plugins = parse_manifest(manifestPath);
        load(good_plugins);

    }

    bool PluginBundle::has(const std::string &pluginName) const {
        return std::ranges::contains(m_pluginNames, pluginName);
    }

    std::vector<std::string> PluginBundle::getPluginNames() const {
        return m_pluginNames;
    }

    std::string PluginBundle::getBundleAuthor() const {
        return m_bundleAuthor;
    }

    std::string PluginBundle::getBundleVersion() const {
        return m_bundleVersion;
    }

    std::string PluginBundle::getBundleComment() const {
        return m_bundleComment;
    }

    std::string PluginBundle::getBundledDatetime() const {
        return m_bundledDatetime;
    }

    bool PluginBundle::isBundleTrusted() const {
        return m_trusted;
    }

    bool PluginBundle::isBundleSigned() const {
        return m_signed;
    }


    PluginBundle::PluginBundle(const std::filesystem::path &filename) :  PluginBundle(filename, PluginLoadPolicy::ALL_PLUGINS_ABI_COMPATIBLE) {}

    PluginBundle::PluginBundle(const std::string &filename) : PluginBundle(std::filesystem::path(filename)) {}

    PluginBundle::PluginBundle(const char *filename) : PluginBundle(std::string(filename)) {}

    PluginBundle::PluginBundle(const std::string &filename, const PluginLoadPolicy policy) : PluginBundle(std::filesystem::path(filename), policy) {}

    PluginBundle::PluginBundle(const char *filename, const PluginLoadPolicy policy) : PluginBundle(std::string(filename), policy) {}

    void PluginBundle::load(const std::vector<PluginPlatforms> &plugins) const {
        for (const auto& plugin: plugins) {
            m_pluginManager.load(m_temporaryDirectory.get_path() / plugin.path);
        }
    }

    void PluginBundle::unpackBundle(const std::filesystem::path& archivePath, const utils::TemporaryDirectory &temporaryDirectory) {
        const std::filesystem::path tempDirectoryPath = temporaryDirectory.get_path();
        unzip_archive(archivePath, tempDirectoryPath);
    }

    std::string PluginBundle::getHostABISignature() {
        #if defined(__linux__)
            // --- LINUX ---
            const char* glibc_version = gnu_get_libc_version();
            if (!glibc_version) {
                throw std::runtime_error("Could not determine glibc version at runtime.");
            }
            return "gcc-libstdc++-" + std::string(glibc_version) + "-cxx11_abi";

        #elif defined(__APPLE__)
            // --- MACOS ---
            char version_str[256] = {0};
            size_t size = sizeof(version_str);
            if (sysctlbyname("kern.osproductversion", version_str, &size, nullptr, 0) != 0) {
                throw std::runtime_error("Could not get macOS version via sysctl.");
            }
            // We use the OS version as the library version for compatibility checking.
            return "clang-libc++-" + std::string(version_str) + "-libc++_abi";

        #else
            #error "This platform is not supported."
            return "unknown-abi";
        #endif
    }

    std::string PluginBundle::getHostArchitecture() {
        struct utsname buffer{};
        if (uname(&buffer) != 0) {
            throw std::runtime_error("uname() failed");
        }
        std::string arch = buffer.machine;

        return arch;
    }

    std::string PluginBundle::getHostOperatingSystem() {
        #if defined(__linux__)
            return "linux";
        #elif defined(__APPLE__)
            return "macos";
        #else
            #error "This platform is not supported."
            return "unknown-os";
        #endif
    }
}
