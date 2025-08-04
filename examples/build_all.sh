#!/bin/bash
# Build script for all libplugin examples

set -e  # Exit on any error

echo "Building libplugin Examples"
echo "================================"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to build an example
build_example() {
    local example_dir="$1"
    local example_name=$(basename "$example_dir")
    
    echo -e "\n${YELLOW}Building $example_name...${NC}"
    
    if [ ! -d "$example_dir" ]; then
        echo -e "${RED}Error: Directory $example_dir not found${NC}"
        return 1
    fi
    
    cd "$example_dir"
    
    # Clean previous build if requested
    if [ "$CLEAN" = "1" ] && [ -d "builddir" ]; then
        echo "  Cleaning previous build..."
        rm -rf builddir
    fi
    
    # Setup build directory
    if [ ! -d "builddir" ]; then
        echo "  Setting up build directory..."
        meson setup builddir
    fi
    
    # Compile
    echo "  Compiling..."
    meson compile -C builddir
    
    if [ $? -eq 0 ]; then
        echo -e "  ${GREEN}✓ $example_name built successfully${NC}"
    else
        echo -e "  ${RED}✗ $example_name build failed${NC}"
        return 1
    fi
    
    cd - > /dev/null
}

# Function to run tests if available
run_example_test() {
    local example_dir="$1"
    local example_name=$(basename "$example_dir")
    
    echo -e "\n${YELLOW}Testing $example_name...${NC}"
    cd "$example_dir"
    
    case "$example_name" in
        "01-simple-plugin")
            if [ -f "../02-simple-host/builddir/simple_host" ] && [ -f "builddir/libsimple_greeter.so" ]; then
                echo "Hello World" | ../02-simple-host/builddir/simple_host builddir/libsimple_greeter.so || true
            fi
            ;;
        "03-math-plugins")
            if [ -f "builddir/math_calculator" ]; then
                echo -e "add 2 3\nquit" | ./builddir/math_calculator builddir/*.so 2>/dev/null || true
            fi
            ;;
        "04-data-processors")
            if [ -f "builddir/data_pipeline" ]; then
                ./builddir/data_pipeline --points 100 builddir/*.so 2>/dev/null || true
            fi
            ;;
    esac
    
    cd - > /dev/null
}

# Parse command line arguments
CLEAN=0
TEST=0
SPECIFIC_EXAMPLE=""

while [[ $# -gt 0 ]]; do
    case $1 in
        --clean|-c)
            CLEAN=1
            shift
            ;;
        --test|-t)
            TEST=1
            shift
            ;;
        --example|-e)
            SPECIFIC_EXAMPLE="$2"
            shift 2
            ;;
        --help|-h)
            echo "Usage: $0 [OPTIONS]"
            echo "Options:"
            echo "  --clean, -c         Clean previous builds"
            echo "  --test, -t          Run basic tests after building"
            echo "  --example, -e NAME  Build only specific example"
            echo "  --help, -h          Show this help"
            echo ""
            echo "Examples:"
            echo "  $0                  Build all examples"
            echo "  $0 --clean         Clean and build all examples"
            echo "  $0 -e 01-simple-plugin  Build only the simple plugin example"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Check for required tools
echo "Checking build dependencies..."
if ! command -v meson &> /dev/null; then
    echo -e "${RED}Error: meson is not installed${NC}"
    echo "Please install meson: pip install meson"
    exit 1
fi

if ! command -v pkg-config &> /dev/null; then
    echo -e "${RED}Error: pkg-config is not installed${NC}"
    echo "Please install pkg-config"
    exit 1
fi

# Check for fourdst_plugin dependency
if ! pkg-config --exists fourdst_plugin; then
    echo -e "${YELLOW}Warning: fourdst_plugin not found via pkg-config${NC}"
    echo "Make sure the libplugin library is installed and PKG_CONFIG_PATH is set correctly"
fi

# List of examples to build
EXAMPLES=(
    "01-simple-plugin"
    "02-simple-host"  
    "03-math-plugins"
    "04-data-processors"
)

# Build specific example or all examples
if [ -n "$SPECIFIC_EXAMPLE" ]; then
    if [[ " ${EXAMPLES[@]} " =~ " ${SPECIFIC_EXAMPLE} " ]]; then
        build_example "$SPECIFIC_EXAMPLE"
        if [ "$TEST" = "1" ]; then
            run_example_test "$SPECIFIC_EXAMPLE"
        fi
    else
        echo -e "${RED}Error: Example '$SPECIFIC_EXAMPLE' not found${NC}"
        echo "Available examples: ${EXAMPLES[*]}"
        exit 1
    fi
else
    # Build all examples
    failed_builds=()
    
    for example in "${EXAMPLES[@]}"; do
        if ! build_example "$example"; then
            failed_builds+=("$example")
        fi
    done
    
    # Run tests if requested
    if [ "$TEST" = "1" ]; then
        echo -e "\n${YELLOW}Running basic tests...${NC}"
        for example in "${EXAMPLES[@]}"; do
            if [[ ! " ${failed_builds[@]} " =~ " ${example} " ]]; then
                run_example_test "$example"
            fi
        done
    fi
    
    # Summary
    echo -e "\n${YELLOW}Build Summary${NC}"
    echo "============="
    
    total=${#EXAMPLES[@]}
    successful=$((total - ${#failed_builds[@]}))
    
    echo "Successfully built: $successful/$total examples"
    
    if [ ${#failed_builds[@]} -gt 0 ]; then
        echo -e "${RED}Failed builds:${NC}"
        for failed in "${failed_builds[@]}"; do
            echo "  - $failed"
        done
        exit 1
    else
        echo -e "${GREEN}All examples built successfully!${NC}"
    fi
fi

echo -e "\n${GREEN}Done!${NC}"
echo ""
echo "To test the examples:"
echo "  cd 02-simple-host"
echo "  ./builddir/simple_host ../01-simple-plugin/builddir/libsimple_greeter.so"
echo ""
echo "  cd 03-math-plugins" 
echo "  ./builddir/math_calculator builddir/*.so"
echo ""
echo "  cd 04-data-processors"
echo "  ./builddir/data_pipeline builddir/*.so"
