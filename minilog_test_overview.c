/* @file minilog_test_overviews.c 
 * @brief this  file is used for testing functionalities from minilog 
 * @author Umar Ba <jUmarB@protonmail.com> <github/Jukoo> 
 * @copyright(c) 2025 
 *  
 */
#include <stdio.h> 
#include <stdlib.h> 
#include "minilog.h"

extern char **environ ; 
int main (int ac  , char **av , char **env)  
{ 
  int s =  minilog_setup();

  
  return EXIT_SUCCESS ; 
}
