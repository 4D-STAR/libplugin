# `fourdst-cli plugin` Tool Documentation

The `fourdst` package includes a command-line interface (CLI) tool designed to help with a number of tasks related
to the 4D-STAR project. One of these is plugin development through the `plugin` command.

## Installation

```bash
pip install fourdst
```
or from source
```bash
git clone https://github.com/4D-STAR/fourdst.git
cd fourdst
pip install .
```

## Overview

The `fourdst-cli plugin` command provides a streamlined way to create new plugins for the fourdst_libplugin. It automates
several tasks involved in plugin development, including:

1. **Parsing C++ header files** using libclang to extract interface definitions
2. **Generating plugin scaffolds** with proper structure and build configuration  
3. **Creating method stubs** for all pure virtual methods in the interface
4. **Setting up build system** with Meson configuration

## Command Structure

```
fourdst-cli plugin COMMAND [COMMAND_OPTIONS]
```

### Global Options

- `--help` - Show help information

### Commands

#### `fourdst-cli plugin init`

Initialize a new plugin project from a C++ interface header.

```bash
fourdst-cli plugin init PROJECT_NAME --header HEADER_FILE [OPTIONS]
```

**Arguments:**
- `PROJECT_NAME` - The name of the new plugin project (required)

**Options:**
- `--header, -H` - Path to C++ header file defining the plugin interface (required)
- `--directory, -d` - Directory to create the project in (default: current directory)
- `--version, --ver` - Initial SemVer version of the plugin (default: "0.1.0")

## Detailed Usage

### Basic Plugin Creation

```bash
# Create a simple plugin project
fourdst-cli plugin init my_greeter --header /path/to/greeter_interface.h

# Create in specific directory with custom version
fourdst-cli plugin init my_plugin --header ./interfaces/my_interface.h --directory ./plugins --version "0.2.1"
```

### Interactive Interface Selection

When the header file contains multiple interfaces (classes with pure virtual methods), the CLI will present an interactive selection menu:

```bash
$ fourdst-cli plugin init math_plugin --header math_interfaces.h
Parsing interface header: math_interfaces.h
? Which interface would you like to implement? 
❯ IMathOperation
  IAdvancedMath
  IDataProcessor
```

Use arrow keys to select the desired interface and press Enter.

## Requirements

### System Dependencies

- **libclang** - Required for parsing C++ header files
  ```bash
  pip install libclang
  ```

- **pkg-config** - Required for finding FourDST plugin library
  ```bash
  # Ubuntu/Debian
  sudo apt-get install pkg-config
  
  # macOS (via Homebrew)  
  brew install pkg-config
  ```

### libplugin Plugin Library

The generated projects require the libplugin plugin library to be installed and discoverable via pkg-config:

```bash
# Check if fourdst_plugin is available
pkg-config --exists fourdst_plugin && echo "Found" || echo "Not found"

# Show compiler flags
pkg-config --cflags fourdst_plugin

# Show linker flags  
pkg-config --libs fourdst_plugin
```

If this is not installed then run meson install from the root of the libplugin repository

## Generated Project Structure

The CLI creates a complete plugin project with the following structure:

```
my_plugin/
├── meson.build           # Meson build configuration
├── .gitignore           # Git ignore file
└── src/
    └── my_plugin.cpp    # Plugin implementation with method stubs
```

### Generated Files

#### `meson.build`
- Complete Meson build configuration
- Finds fourdst_plugin dependency via pkg-config
- Sets up shared library target with proper flags
- Includes the interface header directory

#### `src/plugin_name.cpp`
- Complete plugin implementation skeleton
- Includes the parsed interface header
- Implements all pure virtual methods with stubs
- Uses `FOURDST_DECLARE_PLUGIN` macro for plugin registration
- Contains TODO comments for implementation guidance

#### `.gitignore`
- Ignores build directories (`builddir/`)

### Example Generated Code

For an interface like:

```cpp
class IGreeter : public fourdst::plugin::PluginBase {
public:
    virtual ~IGreeter() = default;
    virtual std::string greet(const std::string& name) const = 0;
    virtual void set_style(const std::string& style) = 0;
};
```

The CLI generates:

```cpp
#include "/path/to/greeter_interface.h"

class MyGreeterPlugin : public IGreeter {
public:
    std::string greet(const std::string& name) const override {
        // TODO: Implement the greet method.
        return {};
    }

    void set_style(const std::string& style) override {
        // TODO: Implement the set_style method.
    }
};

FOURDST_DECLARE_PLUGIN(MyGreeterPlugin, "my_greeter", "1.0.0");
```

## Building Generated Projects

After generating a project:

```bash
cd my_plugin
meson setup builddir
meson compile -C builddir
```

The compiled plugin will be available as a shared library (`.so`, `.dylib`, or `.dll` depending on platform) in the `builddir` directory.

## Integration with Examples

The CLI should work well with the provided examples:

```bash
# Generate a plugin for the greeter interface
fourdst-cli plugin init my_greeter --header examples/01-simple-plugin/include/greeter_interface.h

# Generate a math operation plugin
fourdst-cli plugin init my_math --header examples/03-math-plugins/include/math_interfaces.h
```

## Development Workflow

1. **Define your interface** in a header file
2. **Generate plugin scaffold** using `fourdst-cli plugin init`
3. **Implement methods** by replacing TODO comments with actual code
4. **Build and test** using Meson
5. **Load in host application** using the plugin manager

Note that when developing a plugin for use in an external library, you **must** use the interface definition provided by that library.
