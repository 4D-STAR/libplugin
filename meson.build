project('libplugin', 'cpp', meson_version : '>= 1.3.0', version : 'v0.2.0', default_options : ['cpp_std=c++23'])

subdir('build-config')

subdir('src')

if get_option('tests')
    message('Setting up tests for libplugin...')
    subdir('tests')
else
    message('Skipping tests for libplugin...')
endif

if get_option('pkg-config')
    pkg_libs = [libplugin, yaml_cpp_dep, openssl_dep, minizip_dep]


    message('Setting up pkg-config file for libplugin...')
    pkg = import('pkgconfig')
    pkg.generate(
        name: 'libplugin',
        description: 'Plugin module for SERiF and related projects',
        version: meson.project_version(),
        libraries: pkg_libs,
        subdirs: ['fourdst'],
        filebase: 'fourdst_plugin',
        install_dir: join_paths(get_option('libdir'), 'pkgconfig')
    )
endif
