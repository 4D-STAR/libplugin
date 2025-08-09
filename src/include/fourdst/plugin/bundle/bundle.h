/**
 * @file bundle.h
 * @brief Plugin bundle management and loading functionality.
 * 
 * This header defines the PluginBundle class and related types for managing
 * and loading plugin bundles. A plugin bundle is a packaged collection of
 * plugins with metadata, signatures, and platform-specific binaries.
 */

#pragma once

#include "fourdst/plugin/manager/plugin_manager.h"
#include "fourdst/plugin/bundle/utils.h"

#include <string>
#include <filesystem>
#include <vector>
#include <optional>
#include <unordered_map>

#include "yaml-cpp/yaml.h"

namespace fourdst::plugin::bundle {
    /**
     * @brief Platform-specific information for a plugin.
     */
    struct PluginPlatforms {
        std::string name;           ///< Name of the platform
        std::string triplet;        ///< Platform triplet (e.g., x86_64-linux-gnu)
        std::string abiSignature;   ///< ABI signature of the platform
        std::string architecture;   ///< CPU architecture (e.g., x86_64)
        std::string path;           ///< Path to the platform-specific binary
    };

    /**
     * @brief Information about a plugin within a bundle.
     */
    struct BundlePluginInfo {
        std::string pluginName;             ///< Name of the plugin
        std::string pluginVersion;          ///< Version of the plugin
        std::string pluginDynamicLibPath;   ///< Path to the plugin's dynamic library
        std::string pluginCompiledOn;       ///< Timestamp of when the plugin was compiled

        std::vector<std::string> pluginSupportedArch;  ///< List of supported architectures
        std::vector<std::string> pluginSupportedABI;   ///< List of supported ABIs

        std::optional<std::string> pluginChecksum;     ///< Optional checksum of the plugin
    };

    /**
     * @brief Policy for loading plugins with ABI compatibility checks.
     */
    enum class PluginLoadPolicy {
        ALL_PLUGINS_ABI_COMPATIBLE = 0,  ///< Load only if all plugins are ABI compatible
        ANY_PLUGINS_ABI_COMPATIBLE = 1   ///< Load any plugins that are ABI compatible
    };

    /**
     * @brief Manages a bundle of plugins.
     * 
     * This class handles loading, verifying, and managing a bundle of plugins.
     * It supports signed and unsigned bundles, with optional signature verification.
     * 
     * @par Example: Loading and using a plugin bundle
     * @code
     * try {
     *     // Create a bundle with default load policy (ALL_PLUGINS_ABI_COMPATIBLE)
     *     fourdst::plugin::bundle::PluginBundle bundle("example.bundle");
     *     
     *     // Check if a specific plugin exists in the bundle
     *     if (bundle.has("my_plugin")) {
     *         // Load the plugin
     *         bundle.load("my_plugin");
     *         
     *         // Get bundle metadata
     *         std::cout << "Bundle author: " << bundle.getBundleAuthor() << std::endl;
     *         std::cout << "Bundle version: " << bundle.getBundleVersion() << std::endl;
     *     }
     * } catch (const std::exception& e) {
     *     std::cerr << "Error: " << e.what() << std::endl;
     * }
     * @endcode
     */
    class PluginBundle {
    public:
        /**
         * @brief Construct a new PluginBundle from a filename with default load policy.
         * 
         * @param[in] filename Path to the bundle file.
         */
        explicit PluginBundle(const std::string& filename);
        
        /**
         * @brief Construct a new PluginBundle from a C-style filename with default load policy.
         * 
         * @param[in] filename Path to the bundle file as a C-string.
         */
        explicit PluginBundle(const char* filename);
        
        /**
         * @brief Construct a new PluginBundle from a filesystem path with default load policy.
         * 
         * @param[in] filename Path to the bundle file.
         */
        explicit PluginBundle(const std::filesystem::path& filename);

        /**
         * @brief Construct a new PluginBundle with a custom load policy.
         * 
         * @param[in] filename Path to the bundle file.
         * @param[in] policy Load policy for ABI compatibility checks.
         */
        explicit PluginBundle(const std::string& filename, PluginLoadPolicy policy);
        
        /**
         * @brief Construct a new PluginBundle with a custom load policy from a C-string.
         * 
         * @param[in] filename Path to the bundle file as a C-string.
         * @param[in] policy Load policy for ABI compatibility checks.
         */
        explicit PluginBundle(const char* filename, PluginLoadPolicy policy);

        /**
         * @brief Construct a new PluginBundle with a custom load policy from a filesystem path.
         * 
         * @param[in] filename Path to the bundle file.
         * @param[in] policy Load policy for ABI compatibility checks.
         */
        explicit PluginBundle(const std::filesystem::path& filename, PluginLoadPolicy policy);
        
        ~PluginBundle() = default;

        // Prevent copying
        PluginBundle(const PluginBundle&) = delete;
        PluginBundle& operator=(const PluginBundle&) = delete;
        
        // Prevent moving
        PluginBundle(PluginBundle&&) = delete;
        PluginBundle& operator=(PluginBundle&&) = delete;

        /**
         * @brief Check if a plugin exists in the bundle.
         * 
         * @param[in] pluginName Name of the plugin to check.
         * @return true If the plugin exists in the bundle.
         * @return false If the plugin does not exist in the bundle.
         */
        bool has(const std::string& pluginName) const;
        
        /**
         * @brief Load a plugin from the bundle.
         * 
         * @param[in] pluginName Name of the plugin to load.
         * 
         * @throws std::runtime_error If the plugin cannot be loaded.
         */
        void load(const std::string& pluginName) const;

        /**
         * @brief Get a list of all plugin names in the bundle.
         * 
         * @return std::vector<std::string> List of plugin names.
         */
        std::vector<std::string> getPluginNames() const;

        /**
         * @brief Get the bundle author.
         * 
         * @return std::string Author of the bundle.
         */
        std::string getBundleAuthor() const;
        
        /**
         * @brief Get the bundle version.
         * 
         * @return std::string Version string of the bundle.
         */
        std::string getBundleVersion() const;
        
        /**
         * @brief Get the bundle comment.
         * 
         * @return std::string Comment/description of the bundle.
         */
        std::string getBundleComment() const;
        
        /**
         * @brief Get the bundle creation datetime.
         * 
         * @return std::string Datetime string when the bundle was created.
         */
        std::string getBundledDatetime() const;

        /**
         * @brief Check if the bundle is trusted.
         * 
         * A bundle is considered trusted if its signature is valid and
         * the signing key is in the system's trusted keys.
         * 
         * @return true If the bundle is trusted.
         * @return false If the bundle is not trusted.
         */
        bool isBundleTrusted() const;
        
        /**
         * @brief Check if the bundle is signed.
         * 
         * @return true If the bundle has a signature.
         * @return false If the bundle is not signed.
         */
        bool isBundleSigned() const;

    private:
        std::filesystem::path m_filepath;                   ///< Path to the bundle file
        PluginLoadPolicy m_loadPolicy;  ///< Current load policy
        manager::PluginManager& m_pluginManager;            ///< Reference to the plugin manager

        std::string m_hostABISignature;     ///< ABI signature of the host system
        std::string m_hostArchitecture;     ///< Architecture of the host system
        std::string m_hostOperatingSystem;  ///< Operating system of the host
        std::string m_triplet;              ///< System triplet (e.g., x86_64-linux-gnu)

        YAML::Node m_bundleManifest;        ///< Parsed bundle manifest

        std::string m_bundleName;           ///< Name of the bundle
        std::string m_bundleVersion;        ///< Version of the bundle
        std::string m_bundleAuthor;         ///< Author of the bundle
        std::string m_bundleComment;        ///< Comment/description of the bundle
        std::string m_bundledDatetime;      ///< When the bundle was created

        std::optional<std::string> m_bundleAuthorKeyFingerprint;  ///< Fingerprint of the signing key
        std::optional<std::vector<unsigned char>> m_bundleSignature;  ///< Bundle signature

        std::vector<std::string> m_pluginNames;  ///< List of plugin names in the bundle

        bool m_signed;              ///< Whether the bundle is signed
        bool m_trusted;             ///< Whether the bundle is trusted

        utils::TemporaryDirectory m_temporaryDirectory;  ///< Temporary directory for bundle extraction

    private:
        /**
         * @brief Load plugins from the specified platforms.
         * 
         * @param[in] plugins List of platform-specific plugin information.
         */
        void load(const std::vector<PluginPlatforms>& plugins) const;

        /**
         * @brief Unpack a bundle to a temporary directory.
         * 
         * @param[in] archivePath Path to the bundle archive.
         * @param[in] temporaryDirectory Temporary directory to extract to.
         * 
         * @throws std::runtime_error If unpacking fails.
         */
        static void unpackBundle(const std::filesystem::path& archivePath, 
                               const utils::TemporaryDirectory& temporaryDirectory);

        /**
         * @brief Verify the bundle's signature.
         * 
         * @return true If the bundle's signature is valid.
         * @return false If the bundle's signature is invalid or missing.
         */
        bool verify_bundle();

        /**
         * @brief Build metadata about the host system.
         */
        void build_host_metadata();

        /**
         * @brief Parse the bundle manifest.
         * 
         * @param[in] manifestPath Path to the manifest file.
         * @return std::vector<PluginPlatforms> List of platform-specific plugin information.
         * 
         * @throws std::runtime_error If the manifest is invalid or missing required fields.
         */
        std::vector<PluginPlatforms> parse_manifest(const std::filesystem::path& manifestPath);

        /**
         * @brief Get the ABI signature of the host system.
         * 
         * @return std::string ABI signature string.
         */
        [[nodiscard]] static std::string getHostABISignature();
        
        /**
         * @brief Get the architecture of the host system.
         * 
         * @return std::string Architecture string.
         */
        [[nodiscard]] static std::string getHostArchitecture();
        
        /**
         * @brief Get the operating system name of the host.
         * 
         * @return std::string Operating system name.
         */
        [[nodiscard]] static std::string getHostOperatingSystem();
    };
}