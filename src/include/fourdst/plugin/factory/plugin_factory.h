/**
 * @file plugin_factory.h
 * @brief Plugin factory system and macros for creating loadable plugins
 * 
 * This file provides the infrastructure for creating dynamically loadable
 * plugins. It includes platform-specific export macros, base classes for
 * plugin implementation, and convenience macros for plugin declaration.
 */

#pragma once

#include "fourdst/plugin/iplugin.h"

#if defined(__GNUC__) || defined(__clang__)
    /**
     * @brief Platform-specific macro for exporting plugin symbols on GCC/Clang
     * 
     * This macro ensures that plugin entry points are visible in the shared library
     * and marks them as used to prevent optimization removal.
     */
    #define FOURDST_PLUGIN_EXPORT extern "C" __attribute__((visibility("default"))) __attribute__((used))
#else
    /**
     * @brief Platform-specific macro for exporting plugin symbols on other compilers
     */
    #define FOURDST_PLUGIN_EXPORT extern "C"
#endif

namespace fourdst::plugin {

    /**
     * @brief Global plugin name variable
     * 
     * This variable must be defined by each plugin to specify its name.
     * It is automatically declared by the FOURDST_DECLARE_PLUGIN macro.
     */
    extern const char* g_plugin_name;

    /**
     * @brief Global plugin version variable
     * 
     * This variable must be defined by each plugin to specify its version.
     * It is automatically declared by the FOURDST_DECLARE_PLUGIN macro.
     */
    extern const char* g_plugin_version;

    /**
     * @brief Base class providing default implementations for plugin identification
     * 
     * This class provides a convenient base for plugin implementations that
     * automatically implements the IPlugin interface using the global plugin
     * name and version variables. Plugin classes can inherit from this instead
     * of directly implementing IPlugin.
     * 
     * @note Plugins using this base class must use the FOURDST_DECLARE_PLUGIN macro
     *       to properly define the required global variables.
     */
    class PluginBase : public IPlugin {
    public:
        /**
         * @brief Get the plugin name from the global variable
         * 
         * @return const char* The plugin name as defined by g_plugin_name
         * @throw Never throws
         */
        [[nodiscard]] const char* get_name() const override {
            return g_plugin_name;
        }

        /**
         * @brief Get the plugin version from the global variable
         * 
         * @return const char* The plugin version as defined by g_plugin_version
         * @throw Never throws
         */
        [[nodiscard]] const char* get_version() const override {
            return g_plugin_version;
        }
    };

    /**
     * @brief Function pointer type for plugin creation functions
     * 
     * This type defines the signature for the plugin factory function that
     * must be exported by each plugin library as "create_plugin".
     * 
     * @return IPlugin* A pointer to a newly created plugin instance, or nullptr on failure
     */
    typedef IPlugin* (*plugin_creator_t)();

    /**
     * @brief Function pointer type for plugin destruction functions
     * 
     * This type defines the signature for the plugin cleanup function that
     * must be exported by each plugin library as "destroy_plugin".
     * 
     * @param plugin Pointer to the plugin instance to destroy
     */
    typedef void (*plugin_destroyer_t)(IPlugin*);

}

/**
 * @brief Macro to declare a plugin with automatic factory function generation
 * 
 * This macro should be used in exactly one source file per plugin to generate
 * the required factory functions and define the plugin metadata. It creates:
 * - Global variables for plugin name and version
 * - create_plugin() function that instantiates the plugin class
 * - destroy_plugin() function that safely deletes the plugin instance
 * 
 * @param className The C++ class name that implements the plugin interface
 * @param pluginName A string literal containing the plugin's name
 * @param pluginVersion A string literal containing the plugin's version
 * 
 * @note This macro must be used exactly once per plugin library
 * @note The className must be a complete type at the point of macro expansion
 * @note The className must be default-constructible
 * 
 * Example usage:
 * @code
 * class MyPlugin : public fourdst::plugin::PluginBase {
 *     // ... plugin implementation
 * };
 * 
 * FOURDST_DECLARE_PLUGIN(MyPlugin, "my_plugin", "1.0.0");
 * @endcode
 */
#define FOURDST_DECLARE_PLUGIN(className, pluginName, pluginVersion) \
    const char* fourdst::plugin::g_plugin_name = pluginName; \
    const char* fourdst::plugin::g_plugin_version = pluginVersion; \
    FOURDST_PLUGIN_EXPORT fourdst::plugin::IPlugin* create_plugin() { \
        return new className(); \
    } \
    FOURDST_PLUGIN_EXPORT void destroy_plugin(fourdst::plugin::IPlugin* plugin) { \
        delete plugin; \
    }
