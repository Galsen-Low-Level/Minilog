/* @file minilog.c 
 * @brief A minimalistic log print 
 * @author Umar Ba  <jUmarB@protonmail.com> 
 * */

#include<stdlib.h> 
#include<unistd.h> 
#include<stdio.h> 
#include<time.h> 
#include<assert.h> 
#include<string.h>
#include<locale.h> 
#include<errno.h>
#include<stdalign.h> 
#include<sys/syslog.h>
#include<sys/wait.h> 
#include<fcntl.h>
#include<sys/types.h> 
#include<sys/stat.h>  
#include<poll.h> 

#define  __need___va_list  
#include  <stdarg.h> 

#include  "minilog.h"  

extern char *program_invocation_short_name ; 
char *minilog_basename = (char *)  0 ;
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
     minilog_configure(miniparm) ; 
  }

  /*No buffering  on standard output   */
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


int minilog_configure(struct __minilog_initial_param_t *  restrict parm )  
{
   
   /* handle stream record file*/ 
   if (parm->_fstream->_record_file) 
   {
     int link_fds = minilog_create_record_stream_pipeline(parm->_fstream);
     minilog_watchlog(link_fds);
   }

   return  0 ; 
   
}

int  minilog_create_record_stream_pipeline(mr_sync * restrict  source )
{
  if(!source ) return  -EDESTADDRREQ ;
  
  char template[0x64]  = {0} ; 
  strcat(template ,  source->_record_file) ; 
  strcat(template  , "XXXXXX") ;  
  source->_stream_pipe = mktemp(template) ;  
  
  if(!source->_stream_pipe)  
    return errno ; 
  
  if(!(~0 ^  mkfifo(source->_stream_pipe , S_IRUSR  | S_IWUSR))) 
  {
     if((EEXIST ^ errno))
       return errno ; 
  }
   
  
  source->_fd_stream_links  = (open(source->_stream_pipe , O_RDWR) << 8) ;  
  source->_fd_stream_links |= open(source->_record_file , O_CREAT|O_RDWR , S_IRUSR|S_IWUSR) ;
  

  if(!(~0  ^ ((source->_fd_stream_links >> 8) & 0xff))) 
  { 
    if (~0 != (source->_fd_stream_links  & 0xff))   
      close(source->_fd_stream_links &0xff ) ; 

    unlink(source->_stream_pipe) ; 
    return  -EACCES ;
  } 
  
  dup2((source->_fd_stream_links >> 8) , STDERR_FILENO) ; 
  return source->_fd_stream_links ; 
}

void minilog_watchlog(int fds) 
{
   pid_t logspy =  fork() ; 
   if(!(~0 ^ logspy))
   {
      LOGWARN("Cannot  spy on log file to listen changed due to :%s", strerror(*__errno_location())); 
      return ; 
   }
  
   if(!(logspy & 0xffff))
   {
      minilog_tail_forward_sync(fds)  ; /* like tail -f command */
   }

}

static void minilog_tail_forward_sync(int fds)  
{

  struct  pollfd  evtpolling = { 
    .fd =  ((fds >> 8) & 0xff)  ,
    .events=POLLIN, 
    .revents= 0 , 
  };

  int rfd =  (fds  & 0xff) ; 
 
  printf("-> start listening on   child proc : %i \n", getpid());  
  char minilog_buffer_sync[MIBLMT] = {0} ; 
  while(1) 
  {
     int pollingstsatus =  poll(&evtpolling , 1 , ~0)  ; 
     
     if (!(~0 ^ pollingstsatus))
       break ; 

     if (evtpolling.revents & POLLIN) 
     { 
      
       read(evtpolling.fd,minilog_buffer_sync , 10000) ;
      
       fprintf(stdout , "%s" , minilog_buffer_sync) ; 
       write(rfd , minilog_buffer_sync  , strlen(minilog_buffer_sync)) ;  
       bzero(minilog_buffer_sync , 10000) ; 
       evtpolling.revents &=~POLLIN ;  
     }else 
       puts("listening ...") ; 

  }

  exit(1) ; 
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
  
  char text_copy[0xff]= {0} ; 
  strcpy(text_copy , mlg_ext->text) ;  

  ssize_t  bytes  = minilog_perform_locale(mlg_ext->text) ; 
  memcpy( (mlg_ext->text + bytes)  , text_copy , strlen(text_copy)) ;  
 
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
  minilog_basename = (char *) calloc(strlen(program_invocation_short_name)+2 ,1) ;
  if (!minilog_basename)
    return ; 

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
   
   if(minilog_basename) 
     free(minilog_basename) ; 
}
