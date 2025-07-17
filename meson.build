# @author Umar Ba <jUmarB@protonmail.com> 
project('minilog', 'c',
  version : '0.1',
  default_options : ['c_std=gnu99','warning_level=3'])

pkgcfg = import('pkgconfig') 
cc = meson.get_compiler('c') 
add_project_arguments(
  cc.get_supported_arguments(),
  '-DMINILOG_ALLOW_ABORT_ON_FATAL=1', 
  #By default  the log show the log level severity 
  '-DMINILOG_TURN_OFF_SEVERITY=0', 
  language:'c'
  )

inc_dir = include_directories('./')
using_curses = dependency('curses')  

minilog_so   = shared_library('minilog','minilog.c', soversion:meson.project_version() , dependencies:[using_curses])

minilog_so_decl= declare_dependency(
  link_with:minilog_so , 
  include_directories:[inc_dir],
  dependencies:[using_curses] ,
  version:meson.project_version() 
  ) 

executable(meson.project_name(), 
  'minilog_overview.c',
  link_with:[minilog_so])
