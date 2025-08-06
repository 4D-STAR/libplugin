#pragma once

#include "fourdst/plugin/plugin.h"

// This header provides the definitions for specialized plugin interfaces
// used exclusively for testing. Both the test suite and the mock plugins
// must include this file to ensure RTTI compatibility.

// A mock interface for the valid plugin to test type-safe casting.
// It inherits VIRTUALLY from IPlugin to solve the diamond inheritance problem.
class IValidPlugin : public fourdst::plugin::PluginBase {
public:
    using PluginBase::PluginBase;

    ~IValidPlugin() override = default;
    [[nodiscard]] virtual int get_magic_number() const = 0;
};

// A different, incompatible interface for testing type-safety.
// It also inherits VIRTUALLY.
class IOtherInterface : public fourdst::plugin::PluginBase {
public:
    using PluginBase::PluginBase;
    ~IOtherInterface() override = default;
};

struct ExampleContext {
    int value;
    double threshold;
};

// A mock functor interface for testing plugin functionality.
class IExampleFunctor : public fourdst::plugin::templates::FunctorPlugin_T<ExampleContext> {
    using FunctorPlugin_T::FunctorPlugin_T;
};