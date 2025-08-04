# Simple Plugin Example

This example demonstrates the most basic plugin implementation using the FourDST plugin system. It shows how to create a minimal plugin that implements a simple interface.

## Overview

This example includes:
- `IGreeter` interface definition
- `SimpleGreeter` plugin implementation
- Meson build configuration

## Interface Definition

The `IGreeter` interface defines a simple contract:

```cpp
class IGreeter : public fourdst::plugin::PluginBase {
public:
    virtual ~IGreeter() = default;
    virtual std::string greet(const std::string& name) const = 0;
    virtual void set_greeting_style(const std::string& style) = 0;
};
```

## Plugin Implementation

The `SimpleGreeter` plugin provides a basic greeting functionality with customizable styles.

## Building

```bash
# From this directory
meson setup builddir
meson compile -C builddir
```

This will create `libsimple_greeter.so` (or `.dylib` on macOS, `.dll` on Windows) in the `builddir` directory.

## Using with fourdst-cli

You can also generate a similar project using the CLI:

```bash
fourdst-cli plugin init simple_greeter --header ./include/greeter_interface.h
```

## Testing

The built plugin can be tested with the host application in example 02-simple-host:

```bash
cd ../02-simple-host
meson setup builddir
meson compile -C builddir
./builddir/simple_host ../01-simple-plugin/builddir/libsimple_greeter.so
```

## Files

- `include/greeter_interface.h` - Interface definition
- `src/simple_greeter.cpp` - Plugin implementation
- `meson.build` - Build configuration
