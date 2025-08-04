# Math Plugins Example

This example demonstrates a more complex plugin system with multiple plugin types implementing mathematical operations. It shows how to create a plugin-based calculator system.

## Overview

This example includes:
- `IMathOperation` interface for basic arithmetic operations
- `IAdvancedMath` interface for complex mathematical functions
- Multiple plugin implementations (addition, multiplication, trigonometry, etc.)
- A calculator host application that loads and uses math plugins
- Plugin discovery and dynamic loading

## Components

### Interfaces
- **IMathOperation**: Basic binary operations (add, subtract, multiply, divide)
- **IAdvancedMath**: Advanced mathematical functions (sin, cos, log, etc.)

### Plugins
- **BasicMath**: Implements basic arithmetic operations
- **TrigMath**: Implements trigonometric functions
- **LogMath**: Implements logarithmic and exponential functions

### Host Application
- **MathCalculator**: Interactive calculator that uses loaded plugins

## Building

```bash
# Build all plugins and the host application
meson setup builddir
meson compile -C builddir
```

## Usage

```bash
# Run the calculator with all available plugins
./builddir/math_calculator ./builddir/plugins/*.so

# Run with specific plugins
./builddir/math_calculator ./builddir/plugins/libbasic_math.so ./builddir/plugins/libtrig_math.so
```

## Example Session

```
$ ./builddir/math_calculator ./builddir/plugins/*.so
Loading plugins...
Loaded: basic_math v1.0.0 (IMathOperation)
Loaded: trig_math v1.0.0 (IAdvancedMath) 
Loaded: log_math v1.0.0 (IAdvancedMath)

Math Calculator
===============
Available operations:
  Basic: add, subtract, multiply, divide
  Trigonometry: sin, cos, tan
  Logarithmic: log, exp

> add 5 3
Result: 8

> sin 1.5708
Result: 1

> log 2.71828
Result: 1
```

## Using with fourdst-cli

Generate new math plugins:

```bash
# Create a new plugin implementing IMathOperation
fourdst-cli plugin init my_math --header ./include/math_interfaces.h

# Select IMathOperation from the interface list
# Edit the generated implementation
```

## Files

- `include/math_interfaces.h` - Interface definitions
- `plugins/` - Plugin implementations
- `src/math_calculator.cpp` - Host application
- `meson.build` - Build configuration
