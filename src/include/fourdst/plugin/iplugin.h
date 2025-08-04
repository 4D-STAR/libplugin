/**
 * @file iplugin.h
 * @brief Core plugin interface for the FourDST plugin system
 * 
 * This file defines the fundamental interface that all plugins in the
 * FourDST system must implement. It provides the basic contract for
 * plugin identification and versioning.
 */

#pragma once

namespace fourdst::plugin {
    /**
     * @brief Abstract base interface for all plugins
     * 
     * This interface defines the minimum contract that all plugins must fulfill.
     * Every plugin must provide identification information through name and version
     * methods. This interface is designed to be extended by more specific plugin
     * interfaces that define domain-specific functionality.
     * 
     * @note This is a pure abstract interface - it cannot be instantiated directly.
     * @note All derived classes should ensure thread-safety of the implemented methods.
     */
    class IPlugin {
    public:
        /**
         * @brief Virtual destructor to ensure proper cleanup of derived classes
         * 
         * @throw Never throws
         */
        virtual ~IPlugin() = default;

        /**
         * @brief Get the name of the plugin
         * 
         * This method returns a human-readable name that uniquely identifies
         * the plugin within the system. The name is used by the plugin manager
         * for registration and retrieval operations.
         * 
         * @return const char* A null-terminated string containing the plugin name.
         *                     The returned pointer must remain valid for the lifetime
         *                     of the plugin instance.
         * @throw Never throws
         * 
         * @note The returned string should be a compile-time constant or
         *       a static string to ensure lifetime guarantees.
         * @note Plugin names should be unique within a given plugin manager instance.
         */
        [[nodiscard]] virtual const char* get_name() const = 0;

        /**
         * @brief Get the version of the plugin
         * 
         * This method returns a version string that identifies the specific
         * version of the plugin implementation. This can be used for compatibility
         * checking and debugging purposes.
         * 
         * @return const char* A null-terminated string containing the plugin version.
         *                     The returned pointer must remain valid for the lifetime
         *                     of the plugin instance.
         * @throw Never throws
         * 
         * @note The returned string should be a compile-time constant or
         *       a static string to ensure lifetime guarantees.
         * @note Version strings should follow semantic versioning (e.g., "1.0.0").
         */
        [[nodiscard]] virtual const char* get_version() const = 0;
    };
}
