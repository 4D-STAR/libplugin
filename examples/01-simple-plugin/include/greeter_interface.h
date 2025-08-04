/**
 * @file greeter_interface.h
 * @brief Interface definition for greeting plugins
 * 
 * This file defines the IGreeter interface that greeting plugins must implement.
 * It demonstrates a simple plugin interface with both const and non-const methods.
 */

#pragma once

#include <string>
#include "fourdst/plugin/factory/plugin_factory.h"

/**
 * @brief Interface for greeting plugins
 * 
 * This interface defines the contract for plugins that provide greeting functionality.
 * Plugins implementing this interface can generate personalized greetings and
 * support different greeting styles.
 * 
 * @note Implementations should be thread-safe for the const methods
 * @note The greeting style setting may not be thread-safe depending on implementation
 */
class IGreeter : public fourdst::plugin::PluginBase {
public:
    /**
     * @brief Virtual destructor for proper cleanup
     * 
     * @throw Never throws
     */
    virtual ~IGreeter() = default;

    /**
     * @brief Generate a personalized greeting
     * 
     * Creates a greeting message for the specified person using the current
     * greeting style. The format and content depend on the plugin implementation
     * and the currently configured style.
     * 
     * @param name The name of the person to greet
     * @return std::string The formatted greeting message
     * 
     * @throw std::invalid_argument If the name is empty or contains invalid characters
     * @throw std::runtime_error If the greeting cannot be generated due to internal errors
     * 
     * @note This method should be thread-safe
     * @note Empty names may result in a generic greeting or an exception
     */
    virtual std::string greet(const std::string& name) const = 0;

    /**
     * @brief Set the greeting style
     * 
     * Configures the style or format to use for future greetings. Available
     * styles depend on the specific plugin implementation. Common styles might
     * include "formal", "casual", "friendly", etc.
     * 
     * @param style The name of the greeting style to use
     * 
     * @throw std::invalid_argument If the style is not supported by this plugin
     * @throw std::runtime_error If the style cannot be applied due to internal errors
     * 
     * @note This method may not be thread-safe - external synchronization may be required
     * @note Calling this method may affect ongoing or future greet() calls
     */
    virtual void set_greeting_style(const std::string& style) = 0;
};
