project('simple_greeter_plugin', 'cpp',
    version: '1.0.0',
    default_options: [
        'cpp_std=c++20',
        'warning_level=3',
        'werror=true'
    ]
)

# Find the fourdst plugin library
fourdst_plugin_dep = dependency('fourdst_plugin', required: true)

# Include directories
inc_dirs = include_directories('include')

# Plugin source files
plugin_sources = [
    'src/simple_greeter.cpp'
]

# Build the plugin as a shared library
simple_greeter_plugin = shared_library(
    'simple_greeter',
    plugin_sources,
    include_directories: inc_dirs,
    dependencies: fourdst_plugin_dep,
    install: false,  # Set to true if you want to install the plugin
    cpp_args: ['-fPIC']  # Ensure position-independent code
)

# Optional: Create a summary
summary({
    'Plugin Name': 'simple_greeter',
    'Version': meson.project_version(),
    'Install Location': get_option('libdir') / 'fourdst' / 'plugins'
}, section: 'Plugin Information')
