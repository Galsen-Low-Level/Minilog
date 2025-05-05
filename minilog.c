/* @file minilog.c 
 * @brief A minimalistic log print 
 * @author Umar Ba  <jUmarB@protonmail.com> 
 * */

#include  <stdlib.h> 
#include  <stdarg.h> 
#include  <unistd.h> 
#include  <stdio.h> 
#include  <time.h> 
#include  <assert.h> 
#include  <string.h>
#include  <fmtmsg.h>  
#include  <term.h> 
#include  <curses.h> 
#include  "minilog.h"  


int  htftp_lp_setup(void) {
  int  erret =  OK; 
  if( ERR == setupterm(nptr, STDOUT_FILENO , &erret)) 
  { 
    if(~0 == erret) 
      fprintf(stderr , "terminfo database could not be found.\n") ;  

    erret =  ~0;   
  }

  return erret; 
}

int htftp_log(int loglvl ,  const char * restrict fmtstr , ... )
{
  int  log_status = MM_OK ;  
  __gnuc_va_list ap ; 
  __builtin_va_start(ap , fmtstr) ; 

  switch(loglvl) 
  {
    case INFO   : __get_lp_level(INFO)  ; break; 
    case WARN   : __get_lp_level(WARN)  ; break; 
    case ERROR  : __get_lp_level(ERROR) ; break;
    default :
                 log_status=~0; 
                 goto __htftp_log_end ;  
  }
 
  char tmp_fmtstr[1024]  ={0} ;  
  vsnprintf(tmp_fmtstr,  1024 , fmtstr , ap ) ; 
  log_status = __htftp_log("%s" ,tmp_fmtstr)  ;  

  __builtin_va_end(ap);  

  __restore ; 

__htftp_log_end:
  return  MM_OK != log_status  ? ~0 :  0 ;  
}

static int  
__htftp_log(const char * restrict  fmtstr ,  ...) 
{
  
  char  strtime_buffer[1024] = {0} ; 
  htftp_perform_localtime(strtime_buffer) ; 

  __gnuc_va_list ap ; 
  __builtin_va_start(ap  , fmtstr) ; 

  vsprintf((strtime_buffer +strlen(strtime_buffer)) ,   fmtstr , ap ) ;

  int s =  FLOG(MM_INFO, strtime_buffer) ; 
  __builtin_va_end(ap); 
  

  return s ; 
} 

static void 
htftp_perform_localtime(char strtime_buffer  __parmreq_(1024)) 
{
   time_t  tepoch  = time( (time_t*)0 ) ;     
   struct  tm *broken_down_time = localtime(&tepoch);  
   ssize_t readed_bytes = strftime(strtime_buffer , 1024 ,  "%F%T%P : ", broken_down_time) ; 
   assert(!readed_bytes^strlen(strtime_buffer)) ; 
}

