gtest_dep = dependency('gtest', main: true, required : true)
gtest_main = dependency('gtest_main', required: true)
gtest_nomain_dep = dependency('gtest', main: false, required : true)

mock_plugin_link_args = []
if host_machine.system() == 'darwin'
    mock_plugin_link_args += '-Wl,-undefined,dynamic_lookup'
elif host_machine.system() == 'linux'
    mock_plugin_link_args += '-Wl,--unresolved-symbols=ignore-all'
endif


# --- Define Mock Plugin Libraries ---
valid_plugin_lib = shared_library('valid_plugin', 'mocks/valid_plugin.cpp',
                                  include_directories: include,
                                  link_args: mock_plugin_link_args
)
other_plugin_lib = shared_library('other_plugin', 'mocks/other_plugin.cpp',
                                  include_directories: include,
                                  link_args: mock_plugin_link_args
)
no_factory_plugin_lib = shared_library('no_factory_plugin', 'mocks/no_factory_plugin.cpp',
                                       include_directories: include,
                                       link_args: mock_plugin_link_args
)

functor_plugin_lib = shared_library('functor_plugin', 'mocks/functor_plugin.cpp',
                                  include_directories: include,
                                  link_args: mock_plugin_link_args
)

message('[TESTS]: ✅ Valid plugin library setup (will be built): ' + valid_plugin_lib.full_path())
message('[TESTS]: ✅ Other plugin library setup (will be built): ' + other_plugin_lib.full_path())
message('[TESTS]: ✅ No factory plugin library setup (will be build): ' + no_factory_plugin_lib.full_path())
message('[TESTS]: ✅ Functor plugin library setup (will be built): ' + functor_plugin_lib.full_path())

test_sources = [
    'test_spec.cpp',
]

if host_machine.system() == 'darwin' # macOS
    export_dynamic_flag = '-Wl,-export_dynamic'
    message('Using export dynamic flag for macOS: ' + export_dynamic_flag)
elif host_machine.system() == 'linux'
    export_dynamic_flag = '-Wl,--export-dynamic'
    message('Using export dynamic flag for Linux: ' + export_dynamic_flag)
else
    export_dynamic_flag = '' # No flag for other systems like Windows
endif


# Create an executable target for each test
#test_exe = executable(
#    'test_spec',
#    'test_spec.cpp',
#    dependencies: [
#        gtest_dep,
#        gtest_main,
#        plugin_dep,
#    ],
#    cpp_args : [
#        '-DVALID_PLUGIN_PATH="' + valid_plugin_lib.full_path() + '"',
#        '-DNO_FACTORY_PLUGIN_PATH="' + no_factory_plugin_lib.full_path() + '"',
#        '-DOTHER_PLUGIN_PATH="' + other_plugin_lib.full_path() + '"',
#        '-DFUNCTOR_PLUGIN_PATH="' + functor_plugin_lib.full_path() + '"',
#    ],
#    link_args: [
#        export_dynamic_flag,
#    ],
#)
#
#test('test_spec', test_exe, env: ['MESON_SOURCE_ROOT=' + meson.project_source_root(), 'MESON_BUILD_ROOT=' + meson.project_build_root()])

executable(
    'sandbox_test',
    'sandbox/main.cpp',
     dependencies: [
        plugin_dep
    ]
)

