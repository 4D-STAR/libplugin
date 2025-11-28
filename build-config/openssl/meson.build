message('System OpenSSL not found or version is < 3.0.0. Building subproject instead.')
openssl_proj = subproject('openssl')
openssl_dep = openssl_proj.get_variable('openssl_dep').as_static()