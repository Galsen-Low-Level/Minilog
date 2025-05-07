project('minilog', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])

pkgcfg = import('pkgconfig') 
cc = meson.get_compiler('c') 
add_project_arguments(
  cc.get_supported_arguments(), 
  '-DMINILOG_ALLOW_ABORT_ON_FATAL', 
  language:'c'
  )

using_curses = dependency('curses')  

minilog_so   = shared_library('minilog','minilog.c', soversion:meson.project_version() , dependencies:[using_curses])

minilog_so_decl= declare_dependency(include_directories:['./'] ,link_with:minilog_so)  

executable('minilog',
           'minilog_test_overview.c',
           dependencies:[using_curses],
           link_with: minilog_so , 
           install : true)
