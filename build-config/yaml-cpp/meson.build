cmake = import('cmake')
yaml_cpp_cmake_options = cmake.subproject_options()

yaml_cpp_cmake_options.add_cmake_defines({
        'BUILD_SHARED_LIBS': 'ON',
        'YAML_CPP_BUILD_TESTS': 'OFF',
        'CMAKE_CXX_FLAGS': '-Wno-shadow',
        'CMAKE_C_FLAGS': '-Wno-shadow',
        'CMAKE_INSTALL_LIBDIR': get_option('libdir'),
        'CMAKE_INSTALL_INCLUDEDIR': get_option('includedir'),
        'CMAKE_POLICY_VERSION_MINIMUM': '3.5'
    })

yaml_cpp_sp = cmake.subproject(
    'yaml-cpp',
    options: yaml_cpp_cmake_options,
)
yaml_cpp_dep = yaml_cpp_sp.dependency('yaml-cpp')
