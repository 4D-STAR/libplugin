 minizip_dep = dependency('minizip', required: false)
 if not minizip_dep.found()
   minizip_proj = subproject('minizip-ng')
   minizip_dep = minizip_proj.get_variable('minizip_ng_dep')
 endif