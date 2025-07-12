/* @file minilog_test_overviews.c 
 * @brief this  file is used for quick overviews for  minilog 
 * @author Umar Ba <jUmarB@protonmail.com> <github/Jukoo> 
 * @copyright(c) 2025 
 *  
 */
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include "minilog.h"


extern char * minilog_basename ; 


int main (int ac  , char **av , __maybe_unused char **env)  
{ 

  char *mesg="This this my message"; 

  char *file_target =  (char *) 00; 
  if((ac  &  ~(1) ) ) 
      file_target =  *(av +(ac-1)) ; 
  

  mr_sync fsync  = {
     file_target,
     ._comtype= '|' , 
  }; 
 
  int mlgstatus =  minilog_setup(INIT(&fsync)) ; 

  if(!(~0  ^mlgstatus)) 
    return EXIT_FAILURE ; 

   /* This  code below simulate  how your application  will log  message */
   while(1) 
   {
     LOGWARN("This is an Warning  log  with %s  %s   \$%s  %s  \$%s",  mesg ,"another part of mesg" , "please consult the man page" ,
      "now im the part of action " ,  "the tags begins here ");

     sleep(1) ; 
     LOGNTH("This is a basic log  with no effect  sound boring  hunnn ? :x  \$%s","hi folks "); 
     sleep(2) ; 
     LOGINFO("This is an Information  log"); 
     LOGERR("Error log") ; 
     sleep(2) ; 
     LOGARLT("Alert log with blinking effect"); 
     sleep(1) ; 
     //LOGFATAL("FATALITY log  w'll kill  your application  becareful buddy ;)") ; 
     sleep(2) ; 
   }
  

  return EXIT_SUCCESS ; 
}
