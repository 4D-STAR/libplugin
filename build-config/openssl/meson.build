 openssl_dep = dependency('openssl', required: false)
 if not openssl_dep.found()
   openssl_proj = subproject('openssl')
   openssl_dep = openssl_proj.get_variable('openssl_dep')
 endif

