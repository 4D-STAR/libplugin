project('plugin_bundle', 'cpp',
        version: '1.0.0',
        default_options: [
            'cpp_std=c++20',
            'warning_level=3',
        ]
)

# Find the fourdst plugin library
fourdst_plugin_dep = dependency('fourdst_plugin', required: true)


inc_dirs = include_directories('include')

executable(
    'plugin_bundle',
    'src/main.cpp',
    include_directories: inc_dirs,
    dependencies: [fourdst_plugin_dep],
    install: false,
    cpp_args: ['-fPIC', '-Wno-error']  # Disable error promotion
)
