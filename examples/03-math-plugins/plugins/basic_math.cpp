/**
 * @file basic_math.cpp
 * @brief Basic arithmetic operations plugin
 * 
 * This plugin implements fundamental arithmetic operations: addition,
 * subtraction, multiplication, and division with proper error handling.
 */

#include "../include/math_interfaces.h"
#include <stdexcept>
#include <cmath>
#include <algorithm>

/**
 * @brief Plugin implementing basic arithmetic operations
 */
class BasicMath final : public IMathOperation {
private:
    const std::vector<std::string> m_operations = {
        "add", "subtract", "multiply", "divide", "power", "modulo"
    };

public:
    using IMathOperation::IMathOperation;
    std::vector<std::string> get_supported_operations() const override {
        return m_operations;
    }

    double perform_operation(const std::string& operation_name, 
                           double operand1, double operand2) const override {
        if (operation_name == "add") {
            double result = operand1 + operand2;
            if (std::isinf(result) && !std::isinf(operand1) && !std::isinf(operand2)) {
                throw std::overflow_error("Addition overflow");
            }
            return result;
        }
        else if (operation_name == "subtract") {
            double result = operand1 - operand2;
            if (std::isinf(result) && !std::isinf(operand1) && !std::isinf(operand2)) {
                throw std::overflow_error("Subtraction overflow");
            }
            return result;
        }
        else if (operation_name == "multiply") {
            double result = operand1 * operand2;
            if (std::isinf(result) && !std::isinf(operand1) && !std::isinf(operand2)) {
                throw std::overflow_error("Multiplication overflow");
            }
            return result;
        }
        else if (operation_name == "divide") {
            if (operand2 == 0.0) {
                throw std::domain_error("Division by zero");
            }
            double result = operand1 / operand2;
            if (std::isinf(result) && !std::isinf(operand1)) {
                throw std::overflow_error("Division overflow");
            }
            return result;
        }
        else if (operation_name == "power") {
            if (operand1 == 0.0 && operand2 < 0.0) {
                throw std::domain_error("Zero raised to negative power");
            }
            if (operand1 < 0.0 && std::floor(operand2) != operand2) {
                throw std::domain_error("Negative base with non-integer exponent");
            }
            double result = std::pow(operand1, operand2);
            if (std::isinf(result) && !std::isinf(operand1) && !std::isinf(operand2)) {
                throw std::overflow_error("Power operation overflow");
            }
            return result;
        }
        else if (operation_name == "modulo") {
            if (operand2 == 0.0) {
                throw std::domain_error("Modulo by zero");
            }
            return std::fmod(operand1, operand2);
        }
        else {
            throw std::invalid_argument("Unsupported operation: " + operation_name);
        }
    }

    bool supports_operation(const std::string& operation_name) const override {
        return std::find(m_operations.begin(), m_operations.end(), operation_name) 
               != m_operations.end();
    }
};

FOURDST_DECLARE_PLUGIN(BasicMath, "libbasic_math", "1.0.0");
