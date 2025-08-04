project('simple_host', 'cpp',
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

# Application source files
app_sources = [
    'src/main.cpp'
]

# Build the host application
simple_host = executable(
    'simple_host',
    app_sources,
    include_directories: inc_dirs,
    dependencies: fourdst_plugin_dep,
    install: false
)

# Optional: Create a summary
summary({
    'Application Name': 'simple_host',
    'Version': meson.project_version(),
    'Plugin Interface': 'IGreeter'
}, section: 'Host Application Information')
