/**
 * @file functor.h
 * @brief Template interface for functor-style plugins
 * 
 * This file provides a template base class for creating plugins that implement
 * function-like behavior. It's designed for plugins that need to process input
 * data and return transformed output of the same type.
 */

#pragma once

#include "fourdst/plugin/factory/plugin_factory.h"

namespace fourdst::plugin::templates {
    /**
     * @brief Template base class for functor-style plugins
     * 
     * This template provides a convenient base class for plugins that implement
     * functor-like behavior - they take an input of type T and return an output
     * of the same type T. This is particularly useful for data processing,
     * filtering, or transformation plugins.
     * 
     * The template inherits from PluginBase to automatically provide plugin
     * identification functionality while adding the functor interface.
     * 
     * @tparam T The type of data that this functor plugin processes.
     *           Must be copyable and should typically be movable for performance.
     * 
     * @note Implementations should ensure the operator() is thread-safe if
     *       the plugin may be used concurrently
     * @note The input parameter is passed by const reference to avoid unnecessary
     *       copying, while the return is by value to ensure proper ownership
     * 
     * Example usage:
     * @code
     * class DoublePlugin : public FunctorPlugin_T<int> {
     * public:
     *     int operator()(const int& input) const override {
     *         return input * 2;
     *     }
     * };
     * @endcode
     */
    template<typename T>
    class FunctorPlugin_T : public PluginBase {
    public:
        /**
         * @brief Function call operator for processing input data
         * 
         * This pure virtual method must be implemented by derived classes to
         * define the specific transformation or processing logic. The method
         * takes input data and returns processed output of the same type.
         * 
         * @param input The input data to process, passed by const reference
         *              to avoid unnecessary copying
         * 
         * @return T The processed output data. The type is the same as the input
         *           to maintain type consistency in the functor interface
         * 
         * @throw Implementation-dependent. Derived classes should document
         *        any exceptions they may throw during processing.
         * 
         * @note Implementations should be const-correct since this method is const
         * @note Consider noexcept specification in derived classes if the operation
         *       cannot throw exceptions
         * @note For expensive-to-copy types, consider returning by move when possible
         */
        virtual T operator()(const T& input) const = 0;
    };
}