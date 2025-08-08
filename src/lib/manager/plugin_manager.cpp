#include "fourdst/plugin/manager/plugin_manager.h"
#include "fourdst/plugin/factory/plugin_factory.h"

#include <dlfcn.h>
#include <map>
#include <memory>
#include <ranges>
#include <vector>

namespace fourdst::plugin {

    struct PluginDeleter {
        plugin_destroyer_t destroyer;
        void operator()(IPlugin* p) const {
            if (p && destroyer) {
                destroyer(p);
            }
        }
    };

    struct manager::PluginManager::Impl {
        struct PluginHandle {
            std::unique_ptr<IPlugin, PluginDeleter> instance = {nullptr, {nullptr}};
            void* library_handle = nullptr;
        };
        std::map<std::string, PluginHandle> plugins;
    };

    manager::PluginManager::PluginManager() : pimpl(std::make_unique<Impl>()) {}
    manager::PluginManager::~PluginManager() {
        std::vector<std::string> names_to_unload;
        for (const auto &key: pimpl->plugins | std::views::keys) {
            names_to_unload.push_back(key);
        }
        for (const auto& name : names_to_unload) {
            unload(name);
        }
    }

    manager::PluginManager & manager::PluginManager::getInstance() {
        static PluginManager instance;
        return instance;
    }

    void manager::PluginManager::load(const std::filesystem::path& library_path) const {
        if (!std::filesystem::exists(library_path)) {
            throw exception::PluginLoadError("Plugin library not found at path: " + library_path.string());
        }

        void* handle = dlopen(library_path.c_str(), RTLD_LAZY | RTLD_GLOBAL);
        if (!handle) {
            throw exception::PluginLoadError("Failed to load library '" + library_path.string() + "'. Error: " + dlerror());
        }

        auto creator = reinterpret_cast<plugin_creator_t>(dlsym(handle, "create_plugin"));
        auto destroyer = reinterpret_cast<plugin_destroyer_t>(dlsym(handle, "destroy_plugin"));

        if (!creator || !destroyer) {
            dlclose(handle);
            throw exception::PluginSymbolError("Could not find 'create_plugin' or 'destroy_plugin' in library '" + library_path.string() + "'.");
        }

        IPlugin* raw_instance = creator();
        if (!raw_instance) {
            dlclose(handle);
            throw exception::PluginLoadError("Plugin factory in '" + library_path.string() + "' returned a nullptr.");
        }

        const std::string plugin_name = raw_instance->get_name();

        if (pimpl->plugins.contains(plugin_name)) {
            destroyer(raw_instance);
            dlclose(handle);
            throw exception::PluginNameCollisionError("A plugin with the name '" + plugin_name + "' is already loaded.");
        }

        pimpl->plugins[plugin_name].instance = { raw_instance, {destroyer} };
        pimpl->plugins[plugin_name].library_handle = handle;
    }

    void manager::PluginManager::unload(const std::string& plugin_name) const {
        if (const auto it = pimpl->plugins.find(plugin_name); it != pimpl->plugins.end()) {
            it->second.instance.reset();

            dlclose(it->second.library_handle);
            pimpl->plugins.erase(it);
        }
    }

    IPlugin* manager::PluginManager::get_raw(const std::string& plugin_name) const {
        if (const auto it = pimpl->plugins.find(plugin_name); it != pimpl->plugins.end()) {
            return it->second.instance.get();
        }
        return nullptr;
    }

}
