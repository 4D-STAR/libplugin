project('libplugin', 'cpp', meson_version : '>= 1.6.0', version : 'v0.3.6', default_options : ['cpp_std=c++23'])

py = import('python').find_installation(pure: false)

if get_option('python-wheel')
    py_installation = true
    lib_install_dir = py.get_install_dir(pure: false)
    header_install_dir = py.get_install_dir(subdir: 'fourdst/include')
else
    py_installation = false
    lib_install_dir = get_option('libdir')
    header_install_dir = get_option('includedir')
endif


subdir('build-config')

subdir('src')

if get_option('tests')
    build_tests = true
else
    build_tests = false
endif

if build_tests and not py_installation
    subdir('tests')
endif

pkg_config = get_option('pkg-config')

if pkg_config and py_installation
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
