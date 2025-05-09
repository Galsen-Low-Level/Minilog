/* @file minilog_test_overviews.c 
 * @brief this  file is used for quick overviews for  minilog 
 * @author Umar Ba <jUmarB@protonmail.com> <github/Jukoo> 
 * @copyright(c) 2025 
 *  
 */
#include <stdio.h> 
#include <stdlib.h> 
#include "minilog.h"


int main (int ac  , char **av , char **env)  
{ 
  char *mesg="This this my message"; 
  if(minilog_setup()/*! Initial  minilog stuff */)   
    return EXIT_FAILURE ; 

  LOGINFO("This is an Information  log"); 
  LOGWARN("This is an Warning  log  with %s",  mesg); 
  LOGERR("Error log") ; 
  LOGARLT("Alert log with blinking effect"); 
  LOGFATAL("FATALITY log  w'll kill  your application  becareful buddy ;)") ; 
  
  return EXIT_SUCCESS ; 
}
