/* @file minilog.c 
 * @brief A minimalistic log print 
 * @author Umar Ba  <jUmarB@protonmail.com> 
 * */

#include  <stdlib.h> 
#include  <unistd.h> 
#include  <stdio.h> 
#include  <time.h> 
#include  <assert.h> 
#include  <string.h>
#include  <locale.h> 
#include  <errno.h>
#include <sys/syslog.h>

#define  __need___va_list  
#include  <stdarg.h> 
 

#include  "minilog.h"  

extern char *program_invocation_short_name ; 
char minilog_basename[0xff] ={0} ; 


int  minilog_setup(void) {
  
  //!Disable buffering on stdout 
  (void) setvbuf(stdout ,  (char *) 0 ,  _IONBF , 0 ) ; 
  
  int  erret =  OK;
  if( ERR == setupterm(nptr, STDOUT_FILENO , &erret)) 
  {
    switch(erret) 
    {
       case 1  : 
         fprintf(stderr, "Termcap hardcopy not able to use curses\n"); 
         break ; 
       case 0:
         fprintf(stderr, "too few information found to enable curses feature\n"); 
         break ; 
       case ~0 : 
         fprintf(stderr, "Termcapabilities DataBase Not found \n") ;
         break; 
    }
    return   ~(erret & ~erret) ;  
  }

  if(minilog_set_current_locale()) 
  {
    LOGFATAL("Cannot set l18n and l10n"); 
    return ~0; 
  }

  if(!(~(addseverity(5 ,  "ALERT"))) )
  {
     LOGWARN("Not  Able  to add new severity ") ;
  }

  return erret &~erret ; 
}
//!TODO  : Provide a new function that handle multiple category  
static int minilog_set_current_locale(void) 
{
   char *l1xn  = setlocale(LC_TIME , "") ;
   return  !l1xn ? ~0 : 0 ; 
}

int minilog(int loglvl ,  const char * restrict fmtstr , ... )
{
  __gnuc_va_list ap ; 
  __builtin_va_start(ap , fmtstr) ; 
 

  char tmp_fmtstr[MIBLMT]  ={0} ;  
  vsnprintf(tmp_fmtstr,  MIBLMT , fmtstr , ap ) ;
  
  int  log_status =~0 ;
  char * action_tag_symbol  = strchr(tmp_fmtstr, MINILOG_EXTSYMB) ; 
  
  if (!action_tag_symbol)  
  { 
    /** Direct  log  out if the  extra symbole to define  action or tag  
     *  was not found */
    log_status = __minilog(loglvl ,"%s ",tmp_fmtstr); 
    goto __minilog_end_gnu_variadic__; 
  }
 
  /*OtherWise  we provide an extension   to  handle  action and tag field parameters */
  struct __minilog_extended  mlg_ext ; 
  
  memset( &mlg_ext , 0 , sizeof(mlg_ext) ) ; 
  ssize_t to_copy  = (action_tag_symbol - tmp_fmtstr) ;   
 
  memcpy(mlg_ext.text,  tmp_fmtstr ,  to_copy) ;

  if (action_tag_symbol) 
  {  
    /* when  the symbole  detected move one step  cause we don't want to show it  */
    action_tag_symbol++; 

    char *atb[] = { mlg_ext.action ,  mlg_ext.tag } ; 
    int  item = 0 ; 
   
    
    while ( *action_tag_symbol  != 0  ) 
    { 
      char *looking_for_next_symbol= strchr(action_tag_symbol , MINILOG_EXTSYMB) ; 
      if(looking_for_next_symbol) 
      {
         size_t  current_size_buffer =  (looking_for_next_symbol - action_tag_symbol) ; 
         memcpy( *(atb+item) ,  action_tag_symbol , current_size_buffer) ; 
         action_tag_symbol =  (looking_for_next_symbol+1); 
         item =-~item  ; 
         continue  ; 
      }  
      memcpy(*(atb+item) , action_tag_symbol,strlen(action_tag_symbol) ) ;
      *action_tag_symbol=0; 
    }

    __minilog_advanced(loglvl , &mlg_ext) ; 
  }


__minilog_end_gnu_variadic__: 
  __builtin_va_end(ap); 

  return   log_status ==MM_OK ? 0 : ~0 ;  
}

static int 
__minilog_advanced(int loglvl , struct __minilog_extended  * restrict mlg_ext ) 
{
  
  char *text_copy  = strdup(mlg_ext->text) ; 
  
  ssize_t  bytes  = minilog_perform_locale(mlg_ext->text) ; 
  memcpy( (mlg_ext->text + bytes)  , text_copy , strlen(text_copy)) ;  
  free(text_copy) ; 
 
  int severity =  minilog_apply_lglvl(loglvl) ; 
  if(!(~0  ^severity)) 
    FLOG(MM_WARNING ,"Cannot apply  severity scope\n") ; 
 

  //FLOG((severity >> 4 ) ,  mlg_ext)
  //TODO :  Re-think  Flog  
  //        -> option a :  build it like macro   
  //        -> option b :  or bullet proof function
  int s = fmtmsg(MM_CONSOLE|MM_PRINT , minilog_basename ,  
          (severity >> 4 ) , mlg_ext->text,
          strlen(mlg_ext->action) ? mlg_ext->action  :0 , 
          strlen(mlg_ext->tag)    ? mlg_ext->tag     :0 ) ; 

  __restore ; 
  __check_severity(severity); 

  return s ; 
  
}


static int  
__minilog(int loglvl , const char * restrict  fmtstr , ... ) 
{
  char  inline_log_buffer[MIBLMT] = {0}; 
  ssize_t bytes =  minilog_perform_locale(inline_log_buffer) ; 
  
  __gnuc_va_list ap ; 
  __builtin_va_start(ap  , fmtstr) ; 
 

  int severity =  minilog_apply_lglvl(loglvl) ; 
  if(!(~0  ^severity)) 
    FLOG(MM_WARNING ,"Cannot apply  severity scope\n") ; 

  vsprintf((inline_log_buffer+bytes) , fmtstr , ap ) ; 
 
  //!TODO : Add origin basename program  to minilog
  int s =  FLOG((severity >> 4), inline_log_buffer) ;
  
  bzero(inline_log_buffer ,  MIBLMT) ; 

  __builtin_va_end(ap); 
  __restore ; 
  __check_severity(severity); 
  
  return s ; 
} 

static ssize_t 
minilog_perform_locale(char inline_log_buffer  __Nonullable_(MIBLMT)) 
{
   time_t  tepoch  = time( (time_t*)0 ) ;     
   struct  tm *broken_down_time = localtime(&tepoch); 

   //!TODO : Allow user to  define  their  time format ! 
   ssize_t readed_bytes = strftime(inline_log_buffer , MIBLMT ,  "%F %T %P: ", broken_down_time) ; 
   assert(!readed_bytes^strlen(inline_log_buffer)) ; 

   return readed_bytes ; 
}


void minilog_auto_check_program_bn(void) 
{
  char *program_basename = getenv((const char [])  { 0x5f ,  0x00 }) ; 
  if (!program_basename) 
  {
     fprintf(stderr , "Not Able  to define basename program") ; 
     return ; 
  }
  *(minilog_basename)  = 0x3a ; 

  memcpy( (minilog_basename+1)  , program_invocation_short_name , strlen(program_invocation_short_name))  ;  
}

#if defined(MINILOG_ABORT_ON_FATALITY) 
static void  __check_severity(int __severity)  
{ 
  /*! Check  special severity flags */ 
  int check_special_severity = (__severity & 0xf); 
  switch(check_special_severity)
  {
     case FATL: exit(FATL) ;
  }
}

#endif
