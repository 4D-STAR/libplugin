# libplugin

libplugin is a simple, dynamic library based, plugin framework developed for
SERiF, a 4D-STAR project. libplugin has been developed with the primary goal of
being easy and type safe for plugin developers to use.

# Funding

libplugin is a part of the 4D-STAR collaboration.
4D-STAR is funded by European Research Council (ERC) under the Horizon Europe
programme (Synergy Grant agreement No. 101071505: 4D-STAR) Work for this
project is funded by the European Union. Views and opinions expressed are
however those of the author(s) only and do not necessarily reflect those of the
European Union or the European Research Council.

# Installation

The primary installation channel for libplugin is directly from source. Note that you will need a C++ compiler supporting C++23.

```bash
git clone https://github.com/4D-STAR/libplugin
cd libplugin
meson setup build
meson compile -C build
meson test -C build
meson install -C build
```

# Examples
There are robust examples in the `examples/` directory. Users are strongly
encoraged to make use of these examples as learning tools.

# Usage
The primary usage flow for libplugin is

1. Define some common and pure virtual plugin interface as a decendent of
   `fourdst::plugin::PluginBase`. Generally this will be defined by whatever
program expects the plugins to be made for it. That is to say that if I was the
maintainer of the `GridFire` project and I wanted users to be able to make
plugins for `GridFire::solver::DirectNetworkSolver::RHSManager::observe()` I
would need to make some header file which defined the plugin class interface.
2. The plugin author must then make a specific implimentation of that
   interface. So if `GridFire` presents an interface called
`RHSManagerObservePlugin` then the plugin author must make a concrete
implimentation of that pure virtual interface.
3. The plugin author compiles their plugin as a dynamic library (`*.dylib` on
   macOS, `*.so` on linux). They must tell the compiler to include the
libplugin headers (and libplugin library) of course, but also the common
interface header.
4. The "Host" program (in the above examples this would be `GridFire`) must
then be somehow informed of the location of the dynamic library. The method
to do this is up to the Host program to impliment (this could be something like
command line arguments or a config file)
5. Within the Host program the
   `fourdst::plugin::manager::PluginManager::load("path.dylib")` must then be
called for that path. Again it is on the Host program to impliment the details
of how this path is recived (though presumably it should be a runtime argument
with safe handling for if no path is recived.)

## Examples
A very simple example follows

### Host Program
We start with what the host program needs. Remember this includes 

- Some common and pure virtual interface for what kind of plugin it will expect
- A way to get the path to a dynamic library from a plugin author

#### Host Program Entry Point
```c++
// File main.cpp
// This is intended to be the entry point for the host

#include <fourdst/plugin/plugin.h>
#include "host_interface.h"

int main() {
  fourdst::plugin::manager::PluginManager manager;
  manager.load("libsimple_plugin.dylib"); // For now I will just assume that the plugin is at this fixed path
  auto* plugin = manager.get<IHostDefinedPlugin>("plugin_main");
  plugin -> say_hello();
}
```

#### Host Program Interface
```c++
// File: host_interface.h
// This is a file the host program must provide so that both it and the plugin author can know about the plugin interface

#pragma once
#include <fourdst/plugin/plugin.h>

class IHostDefinedPlugin : public fourdst::plugin::PluginBase {
public:
  ~HostDefinedPlugin() override = default;
  virtual void say_hello() const = 0;
}
```

### Plugin Program
The plugin author then only needs to write one file

```c++
// File: plugin.cpp
// This is the file a plugin author would write
#include <fourdst/plugin/plugin.h>
#include "host_interface.h" // Note that it is generally up to the host to sort out how to inform the plugin author where this file is...

#include <iostream>

class CustomPlugin : public IHostDefinedPlugin {
  void say_hello() const override {
    std::cout << "Hello, World!\n";
  }
}

// The first argument is the name of the plugin authors implimentation
// The second argument is the name of the plugin that the host will call. The names the host expects to call should be documented *by the host*
// The last argument is the version number
FOURDST_REGISTER_PLUGIN(CustomPlugin, "plugin_main", "1.0.0")
```

### Compiling a plugin
You very well could call the compiler directly; however, we strongly recommend you use a build system to compile your plugins. We use meson

```meson
# File: meson.build
project('simple_plugin, 'cpp', version: '1.0.0', default_options: ['cpp_std=c++23'])

# Find the fourdst plugin library
fourdst_plugin_dep = dependency('fourdst_plugin', required: true)

# We need some way of informing the plugin about the header from the host program
# if your host program has been installed in a find-able manner this could perhapse done like this
# alternitivley you could use meson's shared_library's include_directories directive and point it
# at whatever directory that header file is in
host_program_dep = dependency('host_program', required: true)

simple_plugin = shared_library(
    'simple_plugin',
    'plugin.cpp',
    dependencies: [fourdst_plugin_dep, host_program_dep],
    install: false,  # Set to true if you want to install the plugin
    cpp_args: ['-fPIC']  # Ensure position-independent code
)
```
then you would simply run the standard set of meson commands

```bash
meson setup buildPlugin
meson compile -C buildPlugin
```

now you will have a file in build named `libsimple_plugin.cpp`

## Templates
We include a simple `FunctorPlugin_T<T>` template plugin allowing Host authors
to impliment functor style plugins. This plugin expects that operator() will be overloaded with the signature

```c++
T operator()(T arg);
```
An example usage might be

### Host
#### Host interface file

```c++
// File: TimestepFunctorInterface.h
#include <fourdst/plugin/plugin.h>

struct TimestepContext {
  double time;
  double dt;
  double value;
  size_t step;
}

class ITimestepFunctor : public fourdst::plugin::templates::FunctorPlugin_T<TimestepContext> {};
```

#### Host usage of plugin
```c++
#include <fourdst/plugin/plugin.h>

#include "TimestepFunctorInterface.h"

void foo() {
  fourdst::plugin::manager::PluginManager manager;
  TimestepContext context {54.3, 0.1, 10.892745, 7}
  manager.load("path.dylib");
  auto* plugin = manager.get<ITimestepFunctor>("plugin_A")
  TimestepContext result = (*plugin)(context);
}
```

#### Plugin author usage
```c++
#include <fourdst/plugin/plugin.h>

#include "TimestepFunctorInterface.h"

class TimestepFunctor : public ITimestepFunctor {
  TimestepContext operator()(TimestepContext ctx) {
    std::cout << "Time: " << ctx.time << ", value: " << ctx.value << "\n";
    return ctx;
  }
}
```

## fourdst-cli
The [fourdst](https://github.com/4D-STAR/fourdst) library contains cli tool
named `fourdst-cli`. One function of this tool is to make the lives of plugin
authors much easier. See [here](./examples/FOURDST-CLI.md) for more details but
the basics are covered below.

Once `fourdst-cli` is installed it is invoked using a git like syntax from

```bash
fourdst-cli plugin init PluginName --header <path/to/interface/defined/by/host>
```

This will let setup all the required files so that the author can just go in
and impliment the functions they want to then build with meson.

## Future
We hope to add support for python plugins being called from C++ in the near future.




