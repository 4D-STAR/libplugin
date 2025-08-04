/**
 * @file exceptions.h
 * @brief Exception classes for the FourDST plugin system
 * 
 * This file defines a hierarchy of exception classes used throughout the
 * FourDST plugin system to handle various error conditions that may occur
 * during plugin loading, management, and execution.
 */

#pragma once

#include <string>
#include <utility>

namespace fourdst::plugin::exception {
    /**
     * @brief Base exception class for all plugin-related errors
     * 
     * This is the root exception class from which all other plugin exceptions
     * derive. It provides a standard interface for error messages and inherits
     * from std::exception to integrate with standard C++ exception handling.
     */
    class PluginError : public std::exception {
    public:
        /**
         * @brief Construct a new PluginError with the given error message
         * 
         * @param error_message A descriptive message explaining the error condition
         */
        explicit PluginError(std::string error_message)
            : m_error_message(std::move(error_message)) {}

        /**
         * @brief Get the error message
         * 
         * @return const char* A null-terminated string containing the error message
         * @throw Never throws
         */
        [[nodiscard]] char const* what() const noexcept override {
            return  m_error_message.c_str();
        }
    protected:
        std::string m_error_message; ///< The error message string
    };

    /**
     * @brief Exception thrown when a plugin fails to load
     * 
     * This exception is thrown when:
     * - The plugin library file cannot be found
     * - The library file cannot be opened (e.g., permissions, corruption)
     * - The plugin factory returns a nullptr
     */
    class PluginLoadError final : public PluginError {
        using PluginError::PluginError;
    };

    /**
     * @brief Exception thrown when attempting to access a plugin that hasn't been loaded
     * 
     * This exception is thrown when trying to retrieve a plugin by name
     * but no plugin with that name has been loaded into the manager.
     */
    class PluginNotLoadedError final : public PluginError {
        using PluginError::PluginError;
    };

    /**
     * @brief Exception thrown when required plugin symbols cannot be found
     * 
     * This exception is thrown when a plugin library doesn't contain the
     * required symbols (create_plugin or destroy_plugin functions) or when
     * the symbols cannot be resolved.
     */
    class PluginSymbolError final : public PluginError {
        using PluginError::PluginError;
    };

    /**
     * @brief Exception thrown when attempting to load a plugin with a name that already exists
     * 
     * This exception is thrown when trying to load a plugin whose name
     * conflicts with an already loaded plugin in the same manager instance.
     */
    class PluginNameCollisionError final : public PluginError {
        using PluginError::PluginError;
    };

    /**
     * @brief Exception thrown when a plugin cannot be cast to the requested type
     * 
     * This exception is thrown when attempting to retrieve a plugin with a
     * specific type but the loaded plugin doesn't implement that interface.
     */
    class PluginTypeError final : public PluginError {
        using PluginError::PluginError;
    };
}