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
#include<signal.h> 
#include<poll.h> 

#define  __need___va_list  
#include  <stdarg.h> 

#include  "minilog.h"  

char *minilog_basename = (char *)00 ;

struct   minilog_pipeline_stream  { 
   char  * _tmpipe;  
   int  _fds ;   
}   mps  = { 
  ._tmpipe = (char *) 00 , 
  0 
} ; 

char  *pipefile = (char *)00 ; 

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
     minilog_configure(miniparm) ;  

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

     /*TODO : register signal  to handle subprocess watcher */
     /*This  i'll be only happen if user decide to follow record file */ 
     
     minilog_watchlog(link_fds); 
     mps._fds =  link_fds ;  
   }

   return  0 ; 
}


int  minilog_create_record_stream_pipeline(mr_sync * restrict  source) 
{
  
  if(!source)
    return -EDESTADDRREQ ;

  //!get type of communication specified 
  switch(minilog_syncom(source->_code) & 0xf) 
  {
    case MINILOG_COM_PIPE: 
      source->_fd_stream_links =  minilog_sync_pipe(source->_record_file) ; 
      break ; 
    case MINILOG_COM_SOCKET:
      /*source->_fd_stream_links =  minilog_sync_socket(source->_record_file);
       *break;*/
      puts("socket");break ; 
  }  

  /* Terminal < -- > [pipe/socket buffer] =={event pollin event}==>  file.log */
  dup2((source->_fd_stream_links >> 8) , STDERR_FILENO) ; 
  return source->_fd_stream_links ; 
}

static int minilog_sync_pipe(const char * restrict  source) 
{
  if(!source) return  ~0 ;
  
  int  fds = 0 ; 
  char *tmplate  = (char *) calloc(strlen(source)+0x8 , (sizeof(char))) ; 
  if (!tmplate)
    return  ~0 ; 

  memset(tmplate , 0x58, (strlen(source)+7))   ; 
  memcpy((tmplate),source , strlen(source)) ; 
  memset((tmplate+strlen(source)) ,  0x2e , 01);

  int  _ = mkstemp(tmplate) ; 
  if(!(~0^_)) 
    return errno ; 

  mps._tmpipe =  strdup(tmplate) ;  
  unlink(tmplate); 
  close(_) ;
  free(tmplate) ; 
  
 
  if(!(~0 ^ mknod(mps._tmpipe,S_IFIFO|S_IRUSR|S_IWUSR,00)))
  {
    perror("mknod") ;  
    if ((EEXIST ^ errno)) 
     {
       fds = errno ; 
       goto  _end ;  
     }
  } 

  fds =  (open(mps._tmpipe,O_RDWR) <<8) | (open(source,O_CREAT|O_APPEND|O_RDWR,S_IRUSR|S_IWUSR)) ; 
  if (!(~0 ^(fds >> 8) & 0x07)) 
  { 
     if(!!(~0  ^ (fds  & 0x0f)))
       close(fds & 0xff ) ;  
     
     goto  _end ; 
  }

_end: 
  return  fds;  
}

int  minilog_watchlog(int fds)    
{
  /*! Register predefined  signal before lauching the process */
  MLOG_DEFSIGCATCH(DEFAULT_TARGET_SIGNALS) ; 

  pid_t subprocess_watcher  =  fork() ;
  if(!(~0 ^ subprocess_watcher))
  {
    LOGWARN("Cannot  spy on log file to listen changed due to :%s", strerror(*__errno_location())); 
    return ~0 ;  
  }
  
  if(!(subprocess_watcher & 0xffff))
    minilog_tail_forward_sync(fds)  ; /* like tail -f command */
  
  return  0 ;  
}

static void minilog_tail_forward_sync(int fds)  
{

  struct  pollfd  evtpolling = { 
    .fd =  ((fds >> 8) & 0xff)  ,
    .events=POLLIN, 
    .revents= 0 , 
  };

  int rfd =  (fds  & 0xff) ; 

  char minilog_buffer_sync[MIBLMT] = {0} ; 
  while(1) 
  {
     int pollingstsatus =  poll(&evtpolling , 1 , ~0)  ; 
     
     if (!(~0 ^ pollingstsatus))
       break ; 

     if (evtpolling.revents & POLLIN) 
     { 
       read(evtpolling.fd,minilog_buffer_sync , MIBLMT) ;
       fprintf(stdout , "%s" , minilog_buffer_sync) ; 
       write(rfd , minilog_buffer_sync  , strlen(minilog_buffer_sync)) ;  
       bzero(minilog_buffer_sync , MIBLMT) ; 
       evtpolling.revents &=~POLLIN ;  
     }  
  }

  exit(1) ; 
}

void  sigcatcher(const int nsigs  , ... )  
{
   __gnuc_va_list ap ; 
   __builtin_va_start(ap , nsigs) ;
   struct sigaction  sigact  ; 
   *(void**) &sigact.sa_handler= minilog_sighdl;
   int sigindex  = ~0 ; 
   while (++sigindex <  nsigs )  
   { 
     int current_signal =  va_arg(ap , int) ;  
     /* TODO: Need to be implemented 
      * !check if the passed signal  is correct 
      * if (!__is_valid_signal(current_signal))
      *     continue ; 
      */
     int sigreg_stat  = sigaction(current_signal , &sigact, nptr) ; 
     
     if(!(~0 ^  sigreg_stat)) 
       LOGWARN("Fail to register this  signal : %s", strsignal(current_signal)); 
   } 
   __builtin_va_end(ap) ;  
}

void minilog_sighdl(int signum )  
{

   int status  = 0 ; 
   waitpid(~0 , &status , WNOHANG); 
   /*!TODO:  Analyse status code  */

   //!_is_terminated_signal(signum)  
   minilog_cleanup();  
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
    /* when  the symbole is  detected move one step forward  cause we don't want to show it */
    action_tag_symbol=(char *)-(~(long long int)action_tag_symbol) ;  

    char *atb[] = { mlg_ext.action ,  mlg_ext.tag } ; 
    int  item = 0 ; 
    
    while ( *action_tag_symbol  != 00  ) 
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


#if  MINILOG_TURN_OFF_SEVERITY 
    /*Disable severity log level*/
    severity &=~severity;  
#endif 
   
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

void minilog_exit(void) 
{
   exit(0);  
}


void minilog_auto_check_program_bn(void) 
{
 
  minilog_basename = (char *) calloc(0x7f ,01) ;
  if (!minilog_basename)
    return ; 
  
  *(minilog_basename)  = 0x3a ; 

#if _ERRNO_H  
  extern char * program_invocation_short_name  ; 

  size_t  pbn_len =  strlen(program_invocation_short_name) ; 
  memcpy( (minilog_basename+1)  , program_invocation_short_name , pbn_len) ; 

#else 

  char *program_basename = getenv((const char [])  {0x5f,00}) ; 
  if (!program_basename) 
  {
     fprintf(stderr , "Not Able  to define basename program"); 
     strcat(minilog_basename ,  "UNDEFINED") ; 
     return ; 
  }

  //!  get root program name 
  char  *path_sep    = (char *)00,
        *penulitmate = (char *)00;

  while (nptr !=(path_sep =strtok(program_basename ,(const char [] ){0x2f,00}  ) ))
  {
     program_basename =(char *) 00; 
     penulitmate = path_sep ;  
  }
  
  memcpy( (minilog_basename+1) , penulitmate , strlen(penulitmate) ) ; 
  
#endif 
}



void minilog_cleanup(void) 
{
  if(mps._tmpipe) 
  {
    unlink(mps._tmpipe); 
    free(mps._tmpipe) ; 
  } 
  if(mps._fds > 0)  
  {
      int  pipestream  = ((mps._fds >> 8) & 0xff) ,
           logfile     = (mps._fds & 0xff); 
      if(0 < pipestream ) 
        close(pipestream) ; 
      if(0 < logfile) 
        close(logfile) ; 
  }
  

   if(minilog_basename) 
     free(minilog_basename) ; 

}
