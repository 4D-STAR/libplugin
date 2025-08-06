project('libplugin', 'cpp', meson_version : '>= 1.3.0', version : 'v0.1.0', default_options : ['cpp_std=c++23'])

subdir('src')

if get_option('tests')
    message('Setting up tests for libplugin...')
    subdir('tests')
else
    message('Skipping tests for libplugin...')
endif

if get_option('pkg-config')
    message('Setting up pkg-config file for libplugin...')
    pkg = import('pkgconfig')
    pkg.generate(
        name: 'libplugin',
        description: 'Plugin module for SERiF and related projects',
        version: meson.project_version(),
        libraries: [libplugin],
        subdirs: ['fourdst'],
        filebase: 'fourdst_plugin',
        install_dir: join_paths(get_option('libdir'), 'pkgconfig')
    )
endif
