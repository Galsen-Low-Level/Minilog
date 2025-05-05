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
#include  <locale.h> 
#include  "minilog.h"  


int  minilog_setup(void) {
  int  erret =  OK; 
  if( ERR == setupterm(nptr, STDOUT_FILENO , &erret)) 
  {
    switch(erret) 
    {
       case 1  : 
         fprintf(stderr, "Termcap hardcopy not able to use cures\n"); 
         break ; 
       case 0:
         fprintf(stderr, "too few information found to enable curses feature\n"); 
         break ; 
       case ~0 : 
         fprintf(stderr, "Termcapabilities DataBase Not found \n") ;
         break; 
    }
    return   ~0 ; 
  }

  //!Disable buffering on stdout 
  (void) setvbuf(stdout ,  (char *) 0 ,  _IONBF , 0 ) ; 
  if(!minilog_set_current_locale()) 
  {
    LOGARLT("Cannot set l18n and l10n"); 
    return ~0; 
  }

  
  return erret &~erret ; 
}

static int minilog_set_current_locale(void) 
{
   char *l1xn  = setlocale(LC_ALL , "") ;
   return  !l1xn ? ~0 : 0 ; 
}

int minilog(int loglvl ,  const char * restrict fmtstr , ... )
{
  __gnuc_va_list ap ; 
  __builtin_va_start(ap , fmtstr) ; 
   char tmp_fmtstr[1024]  ={0} ;  
  vsnprintf(tmp_fmtstr,  1024 , fmtstr , ap ) ; 
  int log_status = __minilog(loglvl ,"%s" ,tmp_fmtstr)  ;  

  __builtin_va_end(ap); 


  return   log_status ==MM_OK ? 0 : ~0 ;  
}

static int  
__minilog(int loglvl , const char * restrict  fmtstr ,  ...) 
{
  
  char  strtime_buffer[1024] = {0} ; 
  minilog_perform_locale(strtime_buffer) ; 

  __gnuc_va_list ap ; 
  __builtin_va_start(ap  , fmtstr) ; 

  //!TODO :  More flexible move 
  //      ->  Allow user to move strtime_buffer  ( LEFT , MIDDLE , RIGHT ) 
  vsprintf((strtime_buffer +strlen(strtime_buffer)) ,   fmtstr , ap ); 

  minilog_apply_lglvl(loglvl) ;
 
  int s =  FLOG(MM_INFO, strtime_buffer) ; 
  __builtin_va_end(ap); 
  
  __restore ; 
  return s ; 
} 

static void 
minilog_perform_locale(char strtime_buffer  __Nonullable_(1024)) 
{
   time_t  tepoch  = time( (time_t*)0 ) ;     
   struct  tm *broken_down_time = localtime(&tepoch);  
   ssize_t readed_bytes = strftime(strtime_buffer , 1024 ,  "%F%T%P : ", broken_down_time) ; 
   assert(!readed_bytes^strlen(strtime_buffer)) ; 
}

