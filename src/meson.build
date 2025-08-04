include = include_directories('include')

lib_src = files(
    'lib/manager/plugin_manager.cpp',
    'lib/utils/plugin_utils.cpp'
)

libplugin = shared_library(
    'plugin',
    lib_src,
    install : true,
    include_directories : include,
    cpp_args : ['-fPIC']
)

plugin_dep = declare_dependency(
    link_with : libplugin,
    include_directories : include
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

install_headers(include_files_base, subdir : 'fourdst/fourdst/plugin' )
install_headers(include_files_exception, subdir : 'fourdst/fourdst/plugin/exception' )
install_headers(include_files_factory, subdir : 'fourdst/fourdst/plugin/factory')
install_headers(include_files_manager, subdir : 'fourdst/fourdst/plugin/manager')
install_headers(include_files_templates, subdir : 'fourdst/fourdst/plugin/templates')
install_headers(include_files_utils, subdir : 'fourdst/fourdst/plugin/utils')