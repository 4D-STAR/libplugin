/**
 * @file log_math.cpp
 * @brief Log functions plugin
 * 
 * This plugin implements logarithmic and exponential functions with
 * proper domain checking and error handling.
 */

#include "../include/math_interfaces.h"
#include <stdexcept>
#include <cmath>
#include <algorithm>

/**
 * @brief Plugin implementing trigonometric and hyperbolic functions
 */
class LogMath final : public IAdvancedMath {
private:
    const std::vector<std::string> m_functions = {
        "log10", "ln", "log2", "exp"
    };

public:
    using IAdvancedMath::IAdvancedMath;
    std::vector<std::string> get_supported_functions() const override {
        return m_functions;
    }

    double evaluate_function(const std::string& function_name, 
                           double argument) const override {
        if (function_name == "log10") {
            if (argument <= 0.0) {
                throw std::domain_error("log10 domain error: argument must be > 0");
            }
            return std::log10(argument);
        }
        else if (function_name == "ln") {
            if (argument <= 0.0) {
                throw std::domain_error("ln domain error: argument must be > 0");
            }
            return std::log(argument);
        }
        else if (function_name == "log2") {
            if (argument <= 0.0) {
                throw std::domain_error("log2 domain error: argument must be > 0");
            }
            return std::log2(argument);
        }
        else if (function_name == "exp") {
            if (argument > 709.782712893384) { // Approximate limit for exp to avoid overflow
                throw std::overflow_error("exp overflow: argument too large");
            }
            return std::exp(argument);
        }
        else {
            throw std::invalid_argument("Unsupported function: " + function_name);
        }
    }

    bool supports_function(const std::string& function_name) const override {
        return std::ranges::find(m_functions, function_name) != m_functions.end();
    }

    std::string get_function_domain(const std::string& function_name) const override {
        if (function_name == "log10" || function_name == "log" || function_name == "log2") {
            return "(0, +∞)";
        }
        else if (function_name == "exp") {
            return "(-∞, +∞)";
        }
        else {
            throw std::invalid_argument("Unknown function: " + function_name);
        }
    }
};

FOURDST_DECLARE_PLUGIN(LogMath, "liblog_math", "1.0.0");
