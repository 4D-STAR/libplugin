# FourDST Plugin System Examples

This directory contains comprehensive examples demonstrating various use cases of the FourDST plugin system. The examples range from basic plugin creation to advanced host applications that load and manage multiple plugins.

## Quick Start with fourdst-cli

The `fourdst-cli` tool helps you quickly scaffold new plugin projects. Install it via pip:

```bash
pip install fourdst-cli
```

### Basic Usage

```bash
# Initialize a new plugin project
fourdst-cli plugin init my_plugin --header /path/to/interface.h

# The CLI will parse the header file and let you choose which interface to implement
# It will generate a complete Meson project with:
# - meson.build file
# - C++ source file with method stubs
# - .gitignore file
```

### CLI Options

- `--header`, `-H`: Path to the C++ header file defining the plugin interface (required)
- `--directory`, `-d`: Directory to create the project in (default: current directory)
- `--version`, `--ver`: Initial SemVer version of the plugin (default: "0.1.0")

## Example Categories

### 1. Basic Examples
- **[01-simple-plugin](./01-simple-plugin/)**: A minimal plugin implementation
- **[02-simple-host](./02-simple-host/)**: A basic application that loads and uses plugins

### 2. Intermediate Examples
- **[03-math-plugins](./03-math-plugins/)**: Mathematical operation plugins with a calculator host
- **[04-data-processors](./04-data-processors/)**: Data processing pipeline with functor plugins

## Building Examples

Each example contains its own build instructions. Generally:

```bash
cd example-directory
meson setup builddir
meson compile -C builddir
```

## Requirements

- C++20 compatible compiler (GCC 10+, Clang 12+, MSVC 19.29+)
- Meson build system
- fourdst_plugin library (libplugin)
- pkg-config (for finding dependencies)

## Plugin Development Workflow

1. **Define your interface**: Create a header file with pure virtual methods
2. **Use fourdst-cli**: Generate a plugin project scaffold
3. **Implement methods**: Fill in the generated method stubs
4. **Build and test**: Use Meson to build your plugin
5. **Integrate**: Load your plugin in a host application

Note that if you are developing a plugin for use in an external library, you **must** use the interface definition provided by that library.

## Best Practices

- **Thread Safety**: Ensure your plugins are thread-safe if they'll be used concurrently
- **Error Handling**: Use the provided exception classes for consistent error reporting
- **Resource Management**: Properly manage resources in plugin constructors/destructors
- **Versioning**: Use semantic versioning for plugin compatibility
- **Documentation**: Document your plugin interfaces thoroughly

## Common Patterns

### Plugin Interface Definition
```cpp
class IMyPlugin : public fourdst::plugin::PluginBase {
public:
    virtual ~IMyPlugin() = default;
    virtual void do_something() = 0;
    virtual int calculate(int input) const = 0;
};
```

### Plugin Implementation
```cpp
class MyPlugin : public IMyPlugin {
public:
    void do_something() override {
        // Implementation
    }
    
    int calculate(int input) const override {
        return input * 2;
    }
};

FOURDST_DECLARE_PLUGIN(MyPlugin, "my_plugin", "1.0.0");
```

### Host Application
```cpp
fourdst::plugin::manager::PluginManager manager;
manager.load("./plugins/my_plugin.so");
auto* plugin = manager.get<IMyPlugin>("my_plugin");
plugin->do_something();
```

## Troubleshooting

### Common Issues

1. **Plugin not found**: Check that the plugin file exists and has correct permissions
2. **Symbol not found**: Ensure you used the `FOURDST_DECLARE_PLUGIN` macro
3. **Type casting failed**: Verify that your plugin implements the expected interface
4. **Name collision**: Each plugin must have a unique name within a manager instance

### Debugging Tips

- Use `ldd` or `otool -L` to check plugin dependencies
- Enable verbose logging in debug builds
- Use `nm` or `objdump` to inspect exported symbols
- Check that pkg-config can find the fourdst_plugin package