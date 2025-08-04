project('math_plugins', 'cpp',
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

# Plugin sources
basic_math_plugin = shared_library(
    'basic_math',
    'plugins/basic_math.cpp',
    include_directories: inc_dirs,
    dependencies: fourdst_plugin_dep,
    install: false,
    cpp_args: ['-fPIC']
)

trig_math_plugin = shared_library(
    'trig_math',
    'plugins/trig_math.cpp',
    include_directories: inc_dirs,
    dependencies: fourdst_plugin_dep,
    install: false,
    cpp_args: ['-fPIC']
)

log_math_plugin = shared_library(
    'log_math',
    'plugins/log_math.cpp',
    include_directories: inc_dirs,
    dependencies: fourdst_plugin_dep,
    install: false,
    cpp_args: ['-fPIC']
)

# Host application
math_calculator = executable(
    'math_calculator',
    'src/math_calculator.cpp',
    include_directories: inc_dirs,
    dependencies: fourdst_plugin_dep,
    install: false
)

# Create a convenient target to build all plugins
alias_target('plugins', [basic_math_plugin, trig_math_plugin, log_math_plugin])

# Summary
summary({
    'Project': 'Math Plugins Example',
    'Plugins': ['basic_math', 'trig_math', 'log_math'],
    'Host Application': 'math_calculator'
}, section: 'Build Information')
