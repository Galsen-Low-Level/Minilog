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
#include  <locale.h> 
#include  "minilog.h"  

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

  //__append_new_severity("ALERT") ; 
  if(!(~(addseverity(5 ,  "ALERT"))) )
  {
     LOGWARN("Not  Able  to add new severity ") ;
  }

  return erret &~erret ; 
}

static int minilog_set_current_locale(void) 
{
   char *l1xn  = setlocale(LC_TIME , "") ;
   return  !l1xn ? ~0 : 0 ; 
}

int minilog(int loglvl ,  const char * restrict fmtstr , ... )
{
  __gnuc_va_list ap ; 
  __builtin_va_start(ap , fmtstr) ; 
 

  char tmp_fmtstr[1024]  ={0} ;  
  vsnprintf(tmp_fmtstr,  1024 , fmtstr , ap ) ;
  
  int  log_status =~0 ; 
  char *looking_for_action_tag = strchr(tmp_fmtstr,'$') ; 
  
  if (!looking_for_action_tag)  
  {
    log_status = __minilog(loglvl ,"%s",tmp_fmtstr); 
    goto __minilog_end_gnu_variadic__; 
  }
 
  //! Enable minilog extension operation when action or tag are define on log print 
  struct __minilog_extended  mlg_ext ; 
  
  memset( &mlg_ext , 0 , sizeof(mlg_ext) ) ; 
  ssize_t to_copy  = (looking_for_action_tag - tmp_fmtstr) ;   
 
  memcpy(mlg_ext.text,  tmp_fmtstr ,  to_copy) ;
  

  if (looking_for_action_tag) 
  {
    looking_for_action_tag++;  
    char *atb[] = { mlg_ext.action ,  mlg_ext.tag } ; 

    int i  = 0 ; 
    //!search for  tag  
    while ( *looking_for_action_tag  != 0  ) 
    { 
      //!looking for next symbole 
      char *at_buffer_area = strchr(looking_for_action_tag , '$') ; 
      if(at_buffer_area) 
      {
         size_t  at_size_buffer =  (at_buffer_area - looking_for_action_tag) ; 
         memcpy( *(atb+i) ,  looking_for_action_tag ,  at_size_buffer) ; 
         looking_for_action_tag =  (at_buffer_area+1);  
      }else
      {
         memcpy(*(atb+i) , looking_for_action_tag,strlen(looking_for_action_tag) ) ;
         *looking_for_action_tag=0; 
      }

      i=-~i ; 
    }

    __minilog_advanced(loglvl , &mlg_ext) ; 
    
  }


__minilog_end_gnu_variadic__: 
  __builtin_va_end(ap); 


  return   log_status ==MM_OK ? 0 : ~0 ;  
}

static int 
__minilog_advanced(int loglvl , struct __minilog_extended  * restrict mlg_ext) 
{

  char *text_copy  = strdup(mlg_ext->text) ; 
  /*push date format to front  */
  ssize_t  bytes  = minilog_perform_locale(mlg_ext->text) ; 
  memcpy( (mlg_ext->text + bytes)  , text_copy , strlen(text_copy)) ;  
  free(text_copy) ; 
 
  int severity =  minilog_apply_lglvl(loglvl) ; 
  if(!(~0  ^severity)) 
    FLOG(MM_WARNING ,"Cannot apply  severity scope\n") ; 

  int s = fmtmsg(MM_CONSOLE|MM_PRINT ,  ":::" ,  
          (severity >> 4 ) , mlg_ext->text,
          strlen(mlg_ext->action) ? mlg_ext->action  :0 , 
          strlen(mlg_ext->tag)    ? mlg_ext->tag     :0 ) ; 

  __restore ; 
  __check_severity(severity); 

  return s ; 
  
}


static int  
__minilog(int loglvl , const char * restrict  fmtstr ,  ...) 
{
  ///!  mesg ,action , tag  

  
  char  inline_log_buffer[1024] = {0}; 
  ssize_t bytes =  minilog_perform_locale(inline_log_buffer) ; 
  
  __gnuc_va_list ap ; 
  __builtin_va_start(ap  , fmtstr) ; 
 

  int severity =  minilog_apply_lglvl(loglvl) ; 
  if(!(~0  ^severity)) 
    FLOG(MM_WARNING ,"Cannot apply  severity scope\n") ; 

  vsprintf((inline_log_buffer+bytes) , fmtstr , ap ) ; 
 
  //!TODO : Add origin basename program  to minilog
  int s =  FLOG((severity >> 4), inline_log_buffer) ;
  
  bzero(inline_log_buffer ,  1024) ; 

  __builtin_va_end(ap); 
  __restore ; 
  __check_severity(severity); 
  
  return s ; 
} 

static ssize_t 
minilog_perform_locale(char inline_log_buffer  __Nonullable_(1024)) 
{
   time_t  tepoch  = time( (time_t*)0 ) ;     
   struct  tm *broken_down_time = localtime(&tepoch); 

   ssize_t readed_bytes = strftime(inline_log_buffer , 1024 ,  "%F%T%P : ", broken_down_time) ; 
   assert(!readed_bytes^strlen(inline_log_buffer)) ; 

   return readed_bytes ; 
}


void minilog_auto_check_program_bn(void) 
{
  char *program_basename  = getenv((const char[]) { 0x5f , 0x00 }) ;   
  
  memcpy(minilog_basename , program_basename  , strlen(program_basename))  ; 
  strcat(minilog_basename , "::2") ; 
  
  
}

