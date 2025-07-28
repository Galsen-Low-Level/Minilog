# +! This meson build is dedicated for generating  doxygen documentation  

fs  = import('fs') 

summary( {
   'Title':'Documentation  Generator Build',
  },section:'Docs ') 




DOC_BUILDER_ENGINE='doxygen'

docs_engine= find_program(DOC_BUILDER_ENGINE , required:false) 

if docs_engine.found() 
  #+ generate Doxyfile if not exists 
  if not fs.exists('Doxyfile.in')
    out_result  = run_command('doxygen' , '-g', 'Doxyfile.in' , capture:true , check:true) 
    if  0 < out_result.returncode()  
      warning('Fail to generate the Doxyfile')
    endif
  else 
    message('Escape Generating ...') 
  endif 

  
  message('Doxyfile  successfully generated')

  docfg =  configuration_data({
    'PROJECT_NAME'   : 'Minilog',
    'DOXYGEN_OUT_DIR': meson.current_build_dir()/'docs', 
    'DOXYGEN_SOURCE_DIR': '../'
    })


  doxyfile = configure_file(
    input: 'Doxyfile.in',
    output: 'Doxyfile',
    configuration:docfg, 
    install:false, 
    )
  
  docgen = custom_target ( 
    'doxycode', 
     command:['doxygen', meson.current_build_dir()/'Doxyfile'],
     depfile: 'Doxyfile',
     input: [meson.current_build_dir()/'Doxyfile'],
     output:['index.html']
    )
  alias_target('doc', docgen)  
  #NOTE :  meson compile  doc -C build 

else 
  warning('cannot find doc engine build')
endif 


