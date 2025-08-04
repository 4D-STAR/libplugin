/**
 * @file math_interfaces.h
 * @brief Mathematical operation plugin interfaces
 * 
 * This file defines interfaces for mathematical operation plugins.
 * It demonstrates multiple interface types that plugins can implement
 * to provide different categories of mathematical functionality.
 */

#pragma once

#include <string>
#include <vector>
#include "fourdst/plugin/factory/plugin_factory.h"

/**
 * @brief Interface for basic mathematical operations
 * 
 * This interface defines basic binary arithmetic operations that take
 * two operands and return a result. All operations should handle
 * standard floating-point edge cases.
 */
class IMathOperation : public fourdst::plugin::PluginBase {
public:
    /**
     * @brief Virtual destructor for proper cleanup
     */
    virtual ~IMathOperation() = default;

    /**
     * @brief Get the list of supported operation names
     * 
     * @return std::vector<std::string> List of operation names this plugin supports
     */
    virtual std::vector<std::string> get_supported_operations() const = 0;

    /**
     * @brief Perform a mathematical operation
     * 
     * @param operation_name The name of the operation to perform
     * @param operand1 The first operand
     * @param operand2 The second operand
     * @return double The result of the operation
     * 
     * @throw std::invalid_argument If the operation name is not supported
     * @throw std::domain_error If the operands are invalid for the operation
     * @throw std::overflow_error If the result would overflow
     * @throw std::underflow_error If the result would underflow
     */
    virtual double perform_operation(const std::string& operation_name, 
                                   double operand1, double operand2) const = 0;

    /**
     * @brief Check if this plugin supports a specific operation
     * 
     * @param operation_name The name of the operation to check
     * @return bool True if the operation is supported
     */
    virtual bool supports_operation(const std::string& operation_name) const = 0;
};

/**
 * @brief Interface for advanced mathematical functions
 * 
 * This interface defines advanced mathematical functions that typically
 * take a single argument and return a result. This includes trigonometric,
 * logarithmic, exponential, and other transcendental functions.
 */
class IAdvancedMath : public fourdst::plugin::PluginBase {
public:
    /**
     * @brief Virtual destructor for proper cleanup
     */
    virtual ~IAdvancedMath() = default;

    /**
     * @brief Get the list of supported function names
     * 
     * @return std::vector<std::string> List of function names this plugin supports
     */
    virtual std::vector<std::string> get_supported_functions() const = 0;

    /**
     * @brief Evaluate a mathematical function
     * 
     * @param function_name The name of the function to evaluate
     * @param argument The argument to the function
     * @return double The result of the function evaluation
     * 
     * @throw std::invalid_argument If the function name is not supported
     * @throw std::domain_error If the argument is outside the function's domain
     * @throw std::overflow_error If the result would overflow
     * @throw std::underflow_error If the result would underflow
     */
    virtual double evaluate_function(const std::string& function_name, 
                                   double argument) const = 0;

    /**
     * @brief Check if this plugin supports a specific function
     * 
     * @param function_name The name of the function to check
     * @return bool True if the function is supported
     */
    virtual bool supports_function(const std::string& function_name) const = 0;

    /**
     * @brief Get the domain description for a function
     * 
     * @param function_name The name of the function
     * @return std::string Human-readable description of the function's domain
     * 
     * @throw std::invalid_argument If the function name is not supported
     */
    virtual std::string get_function_domain(const std::string& function_name) const = 0;
};
