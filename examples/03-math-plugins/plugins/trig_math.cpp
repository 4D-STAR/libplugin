/**
 * @file trig_math.cpp
 * @brief Trigonometric functions plugin
 * 
 * This plugin implements trigonometric and hyperbolic functions with
 * proper domain checking and error handling.
 */

#include "../include/math_interfaces.h"
#include <stdexcept>
#include <cmath>
#include <algorithm>

/**
 * @brief Plugin implementing trigonometric and hyperbolic functions
 */
class TrigMath final : public IAdvancedMath {
private:
    const std::vector<std::string> m_functions = {
        "sin", "cos", "tan", "asin", "acos", "atan", 
        "sinh", "cosh", "tanh", "asinh", "acosh", "atanh"
    };

public:
    using IAdvancedMath::IAdvancedMath;
    std::vector<std::string> get_supported_functions() const override {
        return m_functions;
    }

    double evaluate_function(const std::string& function_name, 
                           double argument) const override {
        if (function_name == "sin") {
            return std::sin(argument);
        }
        else if (function_name == "cos") {
            return std::cos(argument);
        }
        else if (function_name == "tan") {
            double result = std::tan(argument);
            if (std::isinf(result)) {
                throw std::overflow_error("Tangent overflow (argument near odd multiple of π/2)");
            }
            return result;
        }
        else if (function_name == "asin") {
            if (argument < -1.0 || argument > 1.0) {
                throw std::domain_error("asin domain error: argument must be in [-1, 1]");
            }
            return std::asin(argument);
        }
        else if (function_name == "acos") {
            if (argument < -1.0 || argument > 1.0) {
                throw std::domain_error("acos domain error: argument must be in [-1, 1]");
            }
            return std::acos(argument);
        }
        else if (function_name == "atan") {
            return std::atan(argument);
        }
        else if (function_name == "sinh") {
            double result = std::sinh(argument);
            if (std::isinf(result) && !std::isinf(argument)) {
                throw std::overflow_error("sinh overflow");
            }
            return result;
        }
        else if (function_name == "cosh") {
            double result = std::cosh(argument);
            if (std::isinf(result) && !std::isinf(argument)) {
                throw std::overflow_error("cosh overflow");
            }
            return result;
        }
        else if (function_name == "tanh") {
            return std::tanh(argument);
        }
        else if (function_name == "asinh") {
            return std::asinh(argument);
        }
        else if (function_name == "acosh") {
            if (argument < 1.0) {
                throw std::domain_error("acosh domain error: argument must be >= 1");
            }
            return std::acosh(argument);
        }
        else if (function_name == "atanh") {
            if (argument <= -1.0 || argument >= 1.0) {
                throw std::domain_error("atanh domain error: argument must be in (-1, 1)");
            }
            return std::atanh(argument);
        }
        else {
            throw std::invalid_argument("Unsupported function: " + function_name);
        }
    }

    bool supports_function(const std::string& function_name) const override {
        return std::find(m_functions.begin(), m_functions.end(), function_name) 
               != m_functions.end();
    }

    std::string get_function_domain(const std::string& function_name) const override {
        if (function_name == "sin" || function_name == "cos" || function_name == "tan") {
            return "all real numbers (radians)";
        }
        else if (function_name == "asin" || function_name == "acos") {
            return "[-1, 1]";
        }
        else if (function_name == "atan") {
            return "all real numbers";
        }
        else if (function_name == "sinh" || function_name == "cosh" || 
                 function_name == "tanh" || function_name == "asinh") {
            return "all real numbers";
        }
        else if (function_name == "acosh") {
            return "[1, +∞)";
        }
        else if (function_name == "atanh") {
            return "(-1, 1)";
        }
        else {
            throw std::invalid_argument("Unknown function: " + function_name);
        }
    }
};

FOURDST_DECLARE_PLUGIN(TrigMath, "libtrig_math", "1.0.0");
