/**
 * @file greeter_interface.h
 * @brief Interface definition for greeting plugins (copy for host application)
 * 
 * This is a copy of the greeter interface for use by the host application.
 * In a real project, this would typically be in a shared header or library.
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
 */
class IGreeter : public fourdst::plugin::PluginBase {
public:
    using PluginBase::PluginBase;
    /**
     * @brief Virtual destructor for proper cleanup
     */
    virtual ~IGreeter() = default;

    /**
     * @brief Generate a personalized greeting
     * 
     * @param name The name of the person to greet
     * @return std::string The formatted greeting message
     * 
     * @throw std::invalid_argument If the name is empty or contains invalid characters
     * @throw std::runtime_error If the greeting cannot be generated due to internal errors
     */
    virtual std::string greet(const std::string& name) const = 0;

    /**
     * @brief Set the greeting style
     * 
     * @param style The name of the greeting style to use
     * 
     * @throw std::invalid_argument If the style is not supported by this plugin
     * @throw std::runtime_error If the style cannot be applied due to internal errors
     */
    virtual void set_greeting_style(const std::string& style) = 0;
};
