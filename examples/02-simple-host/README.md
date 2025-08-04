# Simple Host Application Example

This example demonstrates how to create a host application that loads and uses plugins. It shows the basic pattern for plugin management in a real application.

## Overview

This example includes:
- A simple console application that loads greeting plugins
- Plugin manager usage and error handling
- Interactive demonstration of plugin functionality

## Features

- Load plugins from command line arguments
- Interactive greeting with different styles
- Proper error handling and resource cleanup
- Demonstration of type-safe plugin casting

## Building

```bash
# From this directory
meson setup builddir
meson compile -C builddir
```

## Usage

```bash
# Load and test a plugin
./builddir/simple_host ../01-simple-plugin/builddir/libsimple_greeter.so

# Load multiple plugins (if available)
./builddir/simple_host plugin1.so plugin2.so plugin3.so
```

## Example Session

```
$ ./builddir/simple_host ../01-simple-plugin/builddir/libsimple_greeter.so
Loading plugin: ../01-simple-plugin/builddir/libsimple_greeter.so
Successfully loaded plugin: simple_greeter v1.0.0

Available greeting styles:
1. casual
2. formal
3. friendly
4. professional

Enter your name: Alice
Enter style (1-4): 2
Formal greeting: Good day, Alice. It is a pleasure to make your acquaintance.

Enter your name (or 'quit' to exit): Bob
Enter style (1-4): 1
Casual greeting: Hey Bob! How's it going?
```

## Key Concepts Demonstrated

1. **Plugin Manager Creation**: How to create and configure a plugin manager
2. **Plugin Loading**: Loading plugins from shared library files
3. **Error Handling**: Proper exception handling for plugin operations
4. **Type Safety**: Using template methods for type-safe plugin casting
5. **Resource Management**: Automatic cleanup when the manager is destroyed

## Files

- `src/main.cpp` - Main application source
- `meson.build` - Build configuration
