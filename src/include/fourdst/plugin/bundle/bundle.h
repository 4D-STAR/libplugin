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
    struct PluginPlatforms {
        std::string name;
        std::string triplet;
        std::string abiSignature;
        std::string architecture;
        std::string path;
    };

    struct BundlePluginInfo {
        std::string pluginName;
        std::string pluginVersion;
        std::string pluginDynamicLibPath;
        std::string pluginCompiledOn;

        std::vector<std::string> pluginSupportedArch;
        std::vector<std::string> pluginSupportedABI;

        std::optional<std::string> pluginChecksum;
    };

    enum class PluginLoadPolicy {
        ALL_PLUGINS_ABI_COMPATIBLE = 0,
        ANY_PLUGINS_ABI_COMPATIBLE = 1
    };

    class PluginBundle {
    public:
        explicit PluginBundle(const std::string& filename);
        explicit PluginBundle(const char* filename);
        explicit PluginBundle(const std::filesystem::path& filename);

        explicit PluginBundle(const std::string& filename, PluginLoadPolicy policy);
        explicit PluginBundle(const char* filename, PluginLoadPolicy policy);


        explicit PluginBundle(const std::filesystem::path& filename, PluginLoadPolicy policy);
        ~PluginBundle() = default;

        PluginBundle(const PluginBundle&) = delete;
        PluginBundle& operator=(const PluginBundle&) = delete;
        PluginBundle(PluginBundle&&) = delete;
        PluginBundle& operator=(PluginBundle&&) = delete;

        bool has(const std::string& pluginName) const;
        void load(const std::string& pluginName) const;

        std::vector<std::string> getPluginNames() const;

        std::string getBundleAuthor() const;
        std::string getBundleVersion() const;
        std::string getBundleComment() const;
        std::string getBundledDatetime() const;

        bool isBundleTrusted() const;
        bool isBundleSigned() const;
    private:
        std::filesystem::path m_filepath;
        manager::PluginManager& m_pluginManager;
        std::unordered_map<std::string, bool> m_loadedMap;

        std::string m_hostABISignature;
        std::string m_hostArchitecture;
        std::string m_hostOperatingSystem;
        std::string m_triplet;

        YAML::Node m_bundleManifest;

        std::string m_bundleName;
        std::string m_bundleVersion;
        std::string m_bundleAuthor;
        std::string m_bundleComment;
        std::string m_bundledDatetime;

        std::optional<std::string> m_bundleAuthorKeyFingerprint;

        std::optional<std::vector<unsigned char>> m_bundleSignature;

        std::vector<std::string> m_pluginNames;
        std::unordered_map<std::string, std::string> m_pluginAuthors;
        std::unordered_map<std::string, std::string> m_pluginVersions;

        bool m_signed;
        bool m_trusted;
        PluginLoadPolicy m_loadPolicy;

        utils::TemporaryDirectory m_temporaryDirectory;


    private:
        void load(const std::vector<PluginPlatforms>& plugins) const;

        static void unpackBundle(const std::filesystem::path& archivePath, const utils::TemporaryDirectory& temporaryDirectory);

        bool verify_bundle();

        void build_host_metadata();

        std::vector<PluginPlatforms> parse_manifest(std::filesystem::path manifestPath);

        [[nodiscard]] static std::string getHostABISignature() ;
        [[nodiscard]] static std::string getHostArchitecture() ;
        [[nodiscard]] static std::string getHostOperatingSystem() ;


    };
}