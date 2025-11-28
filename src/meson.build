include = include_directories('include')

lib_src = files(
    'lib/manager/plugin_manager.cpp',
    'lib/utils/plugin_utils.cpp',
    'lib/crypt/public_key.cpp',
    'lib/crypt/crypt_verification.cpp',
    'lib/crypt/sha256.cpp',
    'lib/bundle/bundle.cpp',
    'lib/bundle/utils.cpp'
)

dl_dep = dependency('dl', required: true)

if py_installation
    libplugin = static_library(
        'plugin',
        lib_src,
        install : true,
        include_directories : include,
        dependencies : [dl_dep, openssl_dep, yaml_cpp_dep, minizip_dep],
        cpp_args : ['-fPIC']
    )
else
    libplugin = shared_library(
        'plugin',
        lib_src,
        install : true,
        include_directories : include,
        dependencies : [dl_dep, openssl_dep, yaml_cpp_dep, minizip_dep],
        cpp_args : ['-fPIC']
    )
endif

plugin_dep = declare_dependency(
    link_with : libplugin,
    include_directories : include,
    dependencies : [dl_dep, openssl_dep, yaml_cpp_dep, minizip_dep],
)

include_files_base = files(
    'include/fourdst/plugin/plugin.h',
    'include/fourdst/plugin/iplugin.h',
)
include_files_exception = files(
    'include/fourdst/plugin/exception/exceptions.h',
)
include_files_factory = files(
    'include/fourdst/plugin/factory/plugin_factory.h',
)
include_files_manager = files(
    'include/fourdst/plugin/manager/plugin_manager.h',
)
include_files_templates = files(
    'include/fourdst/plugin/templates/functor.h',
)
include_files_utils = files(
    'include/fourdst/plugin/utils/plugin_utils.h',
)
include_files_crypt = files(
    'include/fourdst/crypt/public_key.h',
    'include/fourdst/crypt/crypt_verification.h',
    'include/fourdst/crypt/openSSL_utils.h',
)

include_files_bundle = files(
    'include/fourdst/plugin/bundle/bundle.h',
    'include/fourdst/plugin/bundle/utils.h',
)

install_headers(include_files_base, subdir : 'fourdst/fourdst/plugin' )
install_headers(include_files_exception, subdir : 'fourdst/fourdst/plugin/exception' )
install_headers(include_files_factory, subdir : 'fourdst/fourdst/plugin/factory')
install_headers(include_files_manager, subdir : 'fourdst/fourdst/plugin/manager')
install_headers(include_files_templates, subdir : 'fourdst/fourdst/plugin/templates')
install_headers(include_files_utils, subdir : 'fourdst/fourdst/plugin/utils')
install_headers(include_files_crypt, subdir : 'fourdst/fourdst/crypt')
install_headers(include_files_bundle, subdir : 'fourdst/fourdst/plugin/bundle')