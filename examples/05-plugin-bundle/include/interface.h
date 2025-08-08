#pragma once

#include <fourdst/plugin/plugin.h>

class Interface : public fourdst::plugin::PluginBase {
public:
    using PluginBase::PluginBase;
    virtual void say_hello() const = 0;
};

