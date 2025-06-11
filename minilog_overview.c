/* @file minilog_test_overviews.c 
 * @brief this  file is used for quick overviews for  minilog 
 * @author Umar Ba <jUmarB@protonmail.com> <github/Jukoo> 
 * @copyright(c) 2025 
 *  
 */
#include <stdio.h> 
#include <stdlib.h> 
#include "minilog.h"


extern char * minilog_basename ; 


int main (int ac  , char **av , char **env)  
{ 

  char *mesg="This this my message"; 

  char *file_target =  (char *) 0 ; 
  if((ac  &  ~(1) ) ) 
      file_target =  *(av +(ac-1)) ; 

 
  int mlgstatus =  minilog_setup(STREAM_ON(file_target)); 

  if(!(~0  ^mlgstatus)) 
    return EXIT_FAILURE ; 

  
  LOGWARN("This is an Warning  log  with %s  %s   \$%s  %s  \$%s",  mesg ,"another part of mesg" , "please consult the man page" ,
      "now im the part of action " ,  "the tags begins here ");
  LOGNTH("This is a basic log  with no effect  sound boring  hunnn ? :x  \$%s","hi folks "); 

  LOGINFO("This is an Information  log"); 
  LOGERR("Error log") ; 
  LOGARLT("Alert log with blinking effect"); 
  LOGFATAL("FATALITY log  w'll kill  your application  becareful buddy ;)") ; 
  

  return EXIT_SUCCESS ; 
}
