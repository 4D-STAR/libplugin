if py_installation
    message('System OpenSSL not found or version is < 3.0.0. Building subproject instead.')
    openssl_proj = subproject('openssl')
    openssl_dep = openssl_proj.get_variable('openssl_dep').as_static()
else
    openssl_dep = dependency('openssl', version: '>=3.0.0', required: false)

    if not openssl_dep.found()
        message('System OpenSSL not found or version is < 3.0.0. Building subproject instead.')
        openssl_proj = subproject('openssl')
        openssl_dep = openssl_proj.get_variable('openssl_dep')
    else
        message('Found suitable system OpenSSL: ' + openssl_dep.version())
    endif
endif