/**
 * @file simple_greeter.cpp
 * @brief Implementation of a simple greeting plugin
 * 
 * This file implements the SimpleGreeter plugin that provides basic greeting
 * functionality with support for different greeting styles.
 */

#include "../include/greeter_interface.h"
#include <stdexcept>
#include <algorithm>
#include <cctype>

/**
 * @brief Simple implementation of the IGreeter interface
 * 
 * This plugin provides basic greeting functionality with support for multiple
 * greeting styles. It demonstrates proper error handling and state management
 * in a plugin implementation.
 */
class SimpleGreeter : public IGreeter {
private:
    std::string m_style = "casual"; ///< Current greeting style

public:
    /**
     * @brief Generate a personalized greeting
     * 
     * @param name The name of the person to greet
     * @return std::string The formatted greeting message
     * 
     * @throw std::invalid_argument If the name is empty or contains only whitespace
     */
    std::string greet(const std::string& name) const override {
        // Validate input
        if (name.empty() || std::all_of(name.begin(), name.end(), ::isspace)) {
            throw std::invalid_argument("Name cannot be empty or contain only whitespace");
        }

        // Generate greeting based on style
        if (m_style == "formal") {
            return "Good day, " + name + ". It is a pleasure to make your acquaintance.";
        } else if (m_style == "casual") {
            return "Hey " + name + "! How's it going?";
        } else if (m_style == "friendly") {
            return "Hello there, " + name + "! Nice to meet you!";
        } else if (m_style == "professional") {
            return "Welcome, " + name + ". We appreciate your presence.";
        } else {
            // Fallback for unknown styles
            return "Hello, " + name + "!";
        }
    }

    /**
     * @brief Set the greeting style
     * 
     * @param style The name of the greeting style to use
     * 
     * @throw std::invalid_argument If the style is not supported
     */
    void set_greeting_style(const std::string& style) override {
        // Validate supported styles
        if (style != "formal" && style != "casual" && 
            style != "friendly" && style != "professional") {
            throw std::invalid_argument("Unsupported greeting style: " + style + 
                ". Supported styles: formal, casual, friendly, professional");
        }
        
        m_style = style;
    }
};

// Declare the plugin with name and version
FOURDST_DECLARE_PLUGIN(SimpleGreeter, "simple_greeter", "1.0.0");
