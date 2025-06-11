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
#include  <stdalign.h> 
#include <sys/syslog.h>
#include <pthread.h> 
#include <fcntl.h>
#include <poll.h> 


#define  __need___va_list  
#include  <stdarg.h> 
 

#include  "minilog.h"  

extern char *program_invocation_short_name ; 
char minilog_basename[0xff] ={0} ; 

int fdstream = ~0; 

int  minilog_setup(struct  __minilog_initial_param_t * __Nullable  miniparm ) { 
  
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
    return   ~0 ;   
  }

  if(miniparm)
  {
    fdstream = __configure(miniparm) ; 
    if (!(~0 ^ fdstream))  
      LOGWARN("minilog startup configuration failed") ;  
  }
  
  (void) setvbuf(stdout ,  (char *) 0 ,  _IONBF , 0 ) ;  
  
  if(minilog_set_current_locale()) 
  {
    LOGFATAL("Cannot set l18n and l10n"); 
    return ~0; 
  }

  if(!(~(addseverity(5 ,  "ALERT"))) )
  {
     LOGWARN("Not  Able  to add new severity ") ;
  }

  return   (erret  & ~erret);    
}


int  __configure(struct __minilog_initial_param_t *  restrict parm )  
{
   if (!parm) return ~0 ;  
   
   /* handle stream record file*/ 
   if (!parm->_record) 
     return ~0 ; 

   /*TODO : How to synchronize io terminal and file stream log*/ 
   int fd_streamrec =  open(parm->_record , O_CREAT|  O_WRONLY | O_APPEND , S_IRUSR | S_IWUSR) ;  
  
   if (!(~0  ^ fd_streamrec))
   {
     LOGWARN("No able to open  file object, No binding : %s ", strerror(*__errno_location())) ;  
     return ~0 ; 
   }

   /*! Add color to  log  file  but hard  to read
    *  Not recommanded if you  want to analyse the log file later on 
    * dup2(fd_streamrec , STDOUT_FILENO) ; 
    */ 
   dup2(fd_streamrec , STDERR_FILENO); 
   
   return  fd_streamrec ; 
   
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
    MLOG(MM_WARNING ,"Cannot apply  severity scope\n") ; 
 

  int s = MLOG(severity , (char *)mlg_ext) ; 
  
  __restore ; 
  __check_severity(severity); 

  return s ; 
  
}


int minilog_register(int severity , char * buffer)    
{

   char *minext_buffer  __attribute__((aligned(alignof(struct __minilog_extended)))) = buffer ;    
   struct __minilog_extended  * mlg_ext = (struct __minilog_extended*)  minext_buffer; 

   /*
    *  !FEAT :if user wan't show the severity level  : todo : hide sevrity by doing (severity &~severity)  disabled 
    *  if(NO_SERVERITY)  
    *    TURN_OFF(severity)  
    */

   /*  severity&=~severity ;   /*INCOMING  FEATURE :  Allow  to disable the severity but no flag specified yet  */
   return fmtmsg(MM_CONSOLE|MM_PRINT , minilog_basename , (severity >> 4)  , 
                 mlg_ext->text, __mlg_isextended(mlg_ext) ) ;  

}

static int  
__minilog(int loglvl , const char * restrict  fmtstr , ... ) 
{
  
  __mlg_ext_init(mlg_ext); 

  ssize_t bytes =  minilog_perform_locale(mlg_ext.text) ; 
  
  __gnuc_va_list ap ; 
  __builtin_va_start(ap  , fmtstr) ; 
 

  int severity =  minilog_apply_lglvl(loglvl) ; 
  if(!(~0  ^severity)) 
    MLOG(MM_WARNING ,"Cannot apply  severity scope\n") ; 

  vsprintf((mlg_ext.text+bytes) , fmtstr , ap ) ; 
  

  int mlg_status  =  MLOG(severity, (char *) &mlg_ext ) ; 

  __builtin_va_end(ap); 
  __restore ; 
  __check_severity(severity); 
  
  return  mlg_status ;  
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


static void minilog_cleanup(void) 
{
   if(fdstream >0 )  
     close(fdstream) ; 
}
