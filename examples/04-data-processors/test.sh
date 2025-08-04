#!/bin/bash
# Simple test script for the data processors example

set -e

echo "Testing Data Processors Example"
echo "==============================="

# Check if built
if [ ! -f "builddir/data_pipeline" ]; then
    echo "Error: data_pipeline not found. Did you run 'meson compile -C builddir'?"
    exit 1
fi

# Create a simple test
echo "Running with synthetic data (100 points, low noise)..."

# if system is darwin use dylib if linux us so
if [[ "$(uname)" == "Darwin" ]]; then
    ./builddir/data_pipeline --points 100 --noise 0.1 builddir/lib*.dylib
elif [[ "$(uname)" == "Linux" ]]; then
    ./builddir/data_pipeline --points 100 --noise 0.1 builddir/lib*.so
else
    echo "Unknown system: $(uname)"
fi

echo ""
echo "Test completed successfully!"
echo ""
echo "Try running with different parameters:"
echo "  ./builddir/data_pipeline --points 500 --noise 0.3 builddir/lib*.so"
echo "  ./builddir/data_pipeline --help"
