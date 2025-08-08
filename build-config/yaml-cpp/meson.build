cmake = import('cmake')
yaml_cpp_cmake_options = cmake.subproject_options()
yaml_cpp_cmake_options.add_cmake_defines({
 'CMAKE_POLICY_VERSION_MINIMUM': '3.5',
 'BUILD_SHARED_LIBS': 'ON',
 'CMAKE_SKIP_INSTALL_RULES': 'ON',
 'YAML_CPP_BUILD_TESTS': 'OFF',
 'CMAKE_CXX_FLAGS': '-Wno-shadow',
 'CMAKE_C_FLAGS':   '-Wno-shadow',
})
yaml_cpp_sp = cmake.subproject(
    'yaml-cpp',
    options: yaml_cpp_cmake_options,
)
yaml_cpp_dep = yaml_cpp_sp.dependency('yaml-cpp')

message('Registering yaml-cpp headers (' + meson.global_source_root() + '/subprojects/yaml-cpp/include/yaml-cpp) for installation...')
yaml_cpp_headers = meson.global_source_root() + '/subprojects/yaml-cpp/include/yaml-cpp'
install_subdir(yaml_cpp_headers, install_dir: get_option('includedir'))
message('Done registering yaml-cpp headers for installation!')
