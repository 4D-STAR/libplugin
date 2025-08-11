/**
 * @file plugin_manager.h
 * @brief Plugin manager for loading, managing, and accessing plugins
 * 
 * This file defines the PluginManager class which provides the core functionality
 * for dynamically loading shared libraries as plugins, managing their lifetime,
 * and providing type-safe access to plugin instances.
 */

#pragma once


#include <filesystem>
#include <memory>
#include <string>

#include "fourdst/plugin/exception/exceptions.h"
#include "fourdst/plugin/iplugin.h"

namespace fourdst::plugin::manager {

    /**
     * @brief Central manager for plugin loading and lifecycle management
     * 
     * The PluginManager class provides a comprehensive interface for working with
     * dynamically loaded plugins. It handles:
     * - Loading plugins from shared library files
     * - Managing plugin lifetimes and cleanup
     * - Providing type-safe access to loaded plugins
     * - Preventing name collisions between plugins
     * 
     * The manager uses the PIMPL idiom to hide implementation details and maintain
     * ABI stability. It automatically handles proper cleanup of all loaded plugins
     * when the manager is destroyed.
     * 
     * @note This class is not copyable or movable to prevent issues with plugin handles
     * @note All plugin access is thread-safe at the manager level, but individual
     *       plugin instances may not be thread-safe
     */
    class PluginManager {
    public:

        static PluginManager& getInstance();

        /**
         * @brief Copy constructor (deleted)
         * 
         * PluginManager instances cannot be copied due to the nature of shared
         * library handles and plugin instance ownership.
         */
        PluginManager(const PluginManager&) = delete;

        /**
         * @brief Copy assignment operator (deleted)
         * 
         * PluginManager instances cannot be copy-assigned due to the nature of
         * shared library handles and plugin instance ownership.
         */
        PluginManager& operator=(const PluginManager&) = delete;

        /**
         * @brief Move constructor (deleted)
         * 
         * PluginManager instances cannot be moved to maintain stability of
         * plugin handles and prevent issues with library cleanup.
         */
        PluginManager(PluginManager&&) = delete;

        /**
         * @brief Move assignment operator (deleted)
         * 
         * PluginManager instances cannot be move-assigned to maintain stability
         * of plugin handles and prevent issues with library cleanup.
         */
        PluginManager& operator=(PluginManager&&) = delete;

        /**
         * @brief Load a plugin from the specified library path
         * 
         * Attempts to load a shared library as a plugin. The library must export
         * the required symbols (create_plugin and destroy_plugin) and the plugin
         * must have a unique name within this manager instance.
         * 
         * @param library_path Path to the shared library file to load
         * 
         * @throw fourdst::plugin::exception::PluginLoadError If the library file
         *        cannot be found, opened, or if the plugin factory returns nullptr
         * @throw fourdst::plugin::exception::PluginSymbolError If the required
         *        symbols (create_plugin/destroy_plugin) cannot be found in the library
         * @throw fourdst::plugin::exception::PluginNameCollisionError If a plugin
         *        with the same name is already loaded
         * 
         * @note The library_path can be absolute or relative to the current working directory
         * @note Once loaded, the plugin will remain in memory until explicitly unloaded
         *       or the manager is destroyed
         */
        void load(const std::filesystem::path& library_path) const;

        /**
         * @brief Unload a plugin by name
         * 
         * Removes the specified plugin from the manager, calls its destructor,
         * and closes the associated shared library handle. If no plugin with
         * the given name exists, this operation is a no-op.
         * 
         * @param plugin_name The name of the plugin to unload
         * 
         * @throw Never throws (safe to call with non-existent plugin names)
         * 
         * @note After unloading, any pointers to the plugin instance become invalid
         * @note The plugin's destructor is guaranteed to be called before the library is closed
         */
        void unload(const std::string& plugin_name) const;

        /**
         * @brief Get a type-safe pointer to a loaded plugin
         * 
         * Retrieves a plugin by name and attempts to cast it to the specified type.
         * The type must inherit from IPlugin and the plugin must have been loaded
         * and must be compatible with the requested type.
         * 
         * @tparam T The plugin interface type to cast to (must inherit from IPlugin)
         * @param plugin_name The name of the plugin to retrieve
         * 
         * @return T* A pointer to the plugin cast to the requested type
         * 
         * @throw fourdst::plugin::exception::PluginNotLoadedError If no plugin
         *        with the given name has been loaded
         * @throw fourdst::plugin::exception::PluginTypeError If the plugin cannot
         *        be cast to the requested type T
         * 
         * @note The returned pointer remains valid until the plugin is unloaded
         *       or the manager is destroyed
         * @note The template parameter T is validated at compile-time to ensure
         *       it inherits from IPlugin
         * 
         * Example usage:
         * @code
         * auto* my_plugin = manager.get<IMyPluginInterface>("my_plugin");
         * @endcode
         */
        template<typename T>
        T* get(const std::string& plugin_name) {
            static_assert(std::is_base_of_v<IPlugin, T>, "T must inherit from IPlugin");

            IPlugin* plugin = get_raw(plugin_name);
            if (!plugin) {
                throw exception::PluginNotLoadedError(plugin_name + " has not been loaded or does not exist (have you called manager.load()?)");
            }

            T* casted_plugin = dynamic_cast<T*>(plugin);
            if (!casted_plugin) {
                throw exception::PluginTypeError("PluginManager::load: plugin " + plugin_name + " is not of type " + typeid(T).name());
            }
            return casted_plugin;
        }

        bool has(const std::string& plugin_name) const;

    private:
        PluginManager();

        ~PluginManager();
        /**
         * @brief Internal method to get raw plugin pointer without type checking
         * 
         * @param plugin_name The name of the plugin to retrieve
         * @return IPlugin* Raw pointer to the plugin, or nullptr if not found
         * @throw Never throws
         */
        [[nodiscard]] IPlugin* get_raw(const std::string& plugin_name) const;

        struct Impl; ///< Forward declaration for PIMPL implementation
        std::unique_ptr<Impl> pimpl; ///< PIMPL pointer to hide implementation details
    };

} // namespace fourdst::plugin
