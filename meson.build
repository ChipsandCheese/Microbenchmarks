project('cnc-tools', 'c',
  version : '0.1',
  default_options : [
    'warning_level=3',
    'buildtype=plain',
    'debug=true',
    'b_lto=true',
    'c_std=gnu11',
])

cc = meson.get_compiler('c')

cc_is_gcc_like = cc.get_argument_syntax() == 'gcc' 
cc_is_msvc_like = cc.get_argument_syntax() == 'msvc' 

if not cc_is_gcc_like and not cc_is_msvc_like
  message('Please use something gcc-like or msvc-like, or update the build system')
endif

threads_dep = dependency('threads')

subdir('Framework')
subdir('CPU Tests')