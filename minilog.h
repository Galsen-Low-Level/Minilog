/* @file logprint.h 
 * @brief a simple logger with termcap embedded  
 * @author  Umar Ba <jUmarB@protonmail.com> 
 *
 *
 *
 * @music: Voyage - Allude ( Electronic Gems  ) 
 */

#if !defined(__MINILOG) 
#define  __MINILOG 

#include <stddef.h>  
#include <curses.h> 
#include <term.h> 
#include <fmtmsg.h>
#include <sys/cdefs.h> 

#include <features.h> 
#define __need___va_list 
#include <stdarg.h>  


#if defined(__cplusplus)
# define  __mlog  extern "C" 
#else
# define  __mlog 
#endif 

#define __Nullable 
#define __Nonullable_(__sizereq) [static __sizereq] 
#define __Nonullable [static  0x1] 

#if __glibc_has_attribute(__unused__) 
# define __maybe_unused  __attribute__((__unused__))
#else 
# define __maybe_unused  
#endif

#if __glibc_has_attribute(destructor)  
#define  __dtor  __attribute__((destructor))
#else
# if MINILOG_COMPILATION_FAIL_ON_FATAL
# error   "No end distroyer to release allocated resources.A leak memory 'll occured"
# else 
# warning "No end destroyer to release allocated resources.A leak memory 'll occured"
/* NOTE  : should  found another way  to release ressources  without relying on
 *         GNU function attribute: 
 *         -> see  on_exit or  atexit */ 

# endif  /* MINILOG_COMPILE_FAIL_ON_FATAL*/  
# define __dtor   /*  NOTHING  */
#endif

/*NOTE : This override indication  use weak  GNU attribute 
 *     : as  marker to indicate  the function can be rewriten  in user program 
 **/ 
#if __glibc_has_attribute(weak) 
# define __user_override  __attribute__((weak)) 
#else 
# define __user_override  /* NOTHING */ 
#endif 


#if defined(__ptr_t) 
# define  nptr (__ptr_t) 00 
#else 
# define  nptr NULL
#endif


#define MINILOG_INLINE_BUFFER_LIMIT  (8<<7) 
#define MIBLMT  MINILOG_INLINE_BUFFER_LIMIT  

#define __msr  [MINILOG_INLINE_BUFFER_LIMIT]

struct   __minilog_extended 
{
   char text   __msr ; 
   char action __msr ;
   char tag    __msr ; 
}; 

#define  __mlg_ext_init(__v_identifier)  \
  struct __minilog_extended  __v_identifier;\
  memset(&__v_identifier ,  0, sizeof(__v_identifier)) 

#define  __mlg_isextended(__mlg_ds) \
     ((*(*__mlg_ds).action !=  0) ? (*__mlg_ds).action:  0),\
     ((*(*__mlg_ds).tag !=  0) ? (*__mlg_ds).tag:  0) 

#if  !defined(MINILOG_EXTSYMB) 
# define  MINILOG_EXTSYMB   0x24 /*$*/ 
#endif 

enum  {
   MESG , 
#define MESG    MESG 
   ACTION, 
#define ACTION  ACTION 
   TAG
#define TAG     TAG 
}; 

#define  MLOG(__severity_level , __minext)\
  minilog_register(__severity_level, __minext) 

#define  __mlog_interupt    0x1 , putchar 
#define  mlog_exec(__itermcap) tputs(__itermcap , __mlog_interupt)

//!NOTE : For Cursor movement 
#define  mlog_cusror(__x , __y)  mlog_exec(tgoto(cursor_address , __x , __y))

#define  CAPCODE(__code)\
  ((TERMTYPE*)(cur_term))->Strings[__code] 

#define  SETAF   CAPCODE(0x167)  
#define  RSET    CAPCODE(0x27)    
#define  CADDR   CAPCODE(0xa)   
#define  BLINK   CAPCODE(0x1a)   
#define  BOLD    CAPCODE(0x1b)   

#define  __restore  mlog_exec(RSET)  
#define  tc_color_attr(__color_attribute)  mlog_exec(tiparm(SETAF, __color_attribute))   

/* ! Default log level*/
enum __log_level {
  NOTHING, 
#define  LP_NOTHING
  INFO,
#define LP_INFO  tc_color_attr(COLOR_CYAN) 
  WARN, 
#define LP_WARN  tc_color_attr(COLOR_YELLOW) 
  ERROR,
#define LP_ERROR tc_color_attr(COLOR_RED) 
  ALERT, 
#define LP_ALERT mlog_exec(BLINK); tc_color_attr(COLOR_MAGENTA)  
#define MM_ALERT  5  
  FATALITY 
#define LP_FATALITY  mlog_exec(BOLD); LP_ERROR 
} ; 

/*special  log mask */
enum { 
   ALRT  = (1 << 0), 
   FATL  = (1 << 1),
   CRTC  = (1 << 2) /* Not implemented yet */ 
}; 

#define __get_lp_level(__lp_level) LP_##__lp_level 

#define  __LP_GENERIC(__lvl , ...) minilog(__lvl ,__VA_ARGS__)

#define  LOGINFO(...)   __LP_GENERIC(INFO, __VA_ARGS__)  
#define  LOGWARN(...)   __LP_GENERIC(WARN, __VA_ARGS__)  
#define  LOGERR(...)    __LP_GENERIC(ERROR,__VA_ARGS__)  
#define  LOGARLT(...)   __LP_GENERIC(ALERT,__VA_ARGS__)  
#define  LOGFATAL(...)  __LP_GENERIC(FATALITY,__VA_ARGS__)   
#define  LOGNTH(...)    __LP_GENERIC(NOTHING ,__VA_ARGS__)

#if defined(MINILOG_ALLOW_ABORT_ON_FATAL) 
#define MINILOG_ABORT_ON_FATALITY 1  
#endif


#define INIT(...)  \
  &(struct  __minilog_initial_param_t)\
  {\
    __VA_ARGS__\
  }

extern char  *minilog_basname ;  
extern int fdstream  ;  

/** 
 * Representing  the stream communication 
 * should   handled  using Named Pipe 
 * or  using  Local Socket   
 **/
enum __minilog_record_comtype  { 
   PIPE    =  0xC , 
#define   MINILOG_COM_PIPE   PIPE 
/** 
 * NOTE: This part is not implemented yet 
 *       but stay in draft.W'll be available
 *       in the next version 
 */
   SOCKET    
#define MINILOG_COM_SOCKET  SOCKET 
}; 

/** 
 * Data structure  that handle the target logfile specified by the user
 * here is some information  you should awareof 
 * such as the logfilename and the communication type 
 **/
typedef struct   __minilog_record_sync mr_sync ; 
struct __minilog_record_sync   { 
  char * _record_file;            /* The target record file */ 
  char * _stream_pipe;            /* Stream pipe that listen on stderr or stdout  */
  int  _fd_stream_links ;         /* file descriptors for record  file  and streampipe */
  union { 
    unsigned char  _comtype; 
    int            _code:8 ;
  };
};   

/**  
 * Wrap's up some  information and  allow user
 * to determine how  the logfile or  stream syncronisation 
 * should operate  and more. 
 * It's just like  and object  that you can specified  some values 
 * to control how the stream log should behave 
 **/
typedef struct   __minilog_initial_param_t mparm ; 
struct __minilog_initial_param_t { 
   struct __minilog_record_sync *  _fstream; 
   /* ... NOTE: soon ...  */
}; 

typedef  void (*multi_sigcatch)(int  , ...) ;

#if !defined(DEFAULT_TARGET_SIGNALS) 
/* Default registred signal  to handle how the stream log  subprocess  
 * should exit   the program. NOTE: you can specify  more signal. 
 * NOTE:  This Signals are procssed by a variadic function*/
# define  DEFAULT_TARGET_SIGNALS  3,SIGINT,SIGCHLD,SIGTERM 
#endif  


#define  MLOG_DEFSIGCATCH(__nsigs , ...)\
  sigcatcher(__nsigs , ##__VA_ARGS__) 

/**
 * @fn minilog_setup( struct __minilog_initial_param_t * ) ; 
 * @brief configure and  initilize the terminal capbilities and more stuff 
 * @param  struct  __minilog_initial_param_t -  configuration parameters 
 * @return int    -   status code   0 : OK   ; Otherwise error 
 */
__mlog int minilog_setup(struct __minilog_initial_param_t * __Nullable __initial_parameters) ; 

/**
 * @fn  minilog_cleanup(void) 
 * @brief  clean all  allocated memory  when the  application  turned off (exit or brutal exit) 
 **/
void minilog_cleanup(void) __dtor;  

/** 
 * @fn minilog_configure(struct __minilog_initial_param_t *) 
 * @brief  apply or register  the configuration
 * @param  struct __minilog_initial_param_t  * -  initial parameters 
 * @return int    -  0 : OK  ;  Otherwise  error 
 **/
int  minilog_configure(struct __minilog_initial_param_t * __restrict__  __parm)  ; 

/**
 * @fn minilog_create_record_stream_pipeline(mr_sync )  e.g  call it mister sync  :D ! 
 * @brief   create synchronize  stream pipeline between terminal  output and logfile 
 * @param  mr_sync  aka  struct  __minilog_record_sync  - hold the target logfile  e
 * @return   int  -  0 :OK ; Otherwise  error  
 **/
int  minilog_create_record_stream_pipeline(mr_sync * __restrict__  __source);

/**
 * @fn minilog_watchlog(int) 
 * @brief create  the subprocess stream but register the signal before 
 * @param  int   - bit compacted file descritors 
 * @return int   - 0 :OK ; Otherwise error 
 **/
int  minilog_watchlog(int __bitfds) ;  

/** 
 * @fn minilog_sync_pipe(const char * )  
 * @brief establish  record stream to logfile but using pipe communication 
 * @param  const char  *  log filename 
 * @int   int    - 0 : OK ;  Otherwise  error  
 **/
static int minilog_sync_pipe(const char * __restrict__   __source) ; 

/**  
 * <<!>> : Functions marked  <<__user_override>> can be override 
 *         in user  program.  
 * NOTE  : For advanced programmer  can use SHIM Technique.   
 */  

/** 
 * @fn sigcatcher(const int  ,  __gnu_variadic__)  
 * @brief  default built-in signal catcher. Catch desired signal. 
 *         
 * @param  const int n signals, 
 * @param  ...       - signals 
 **/
__mlog void  __user_override 
sigcatcher(const int __nsigs ,  ...) ; 

/** 
 * @fn minilog_sighdl(int) 
 * @brief handle subprocess exiting signal
 * @param   int  - the signal itself 
 **/
__mlog void __user_override 
minilog_sighdl(int __target_signal) ; 

/** 
 * @fn minilog_tail_forward_sync
 * @brief manage to  synchronize the output from the terminal 
 *        and log file 
 * @param  int  -- bits compacted fds 
 */
static void minilog_tail_forward_sync(int __bitfds ) ; 

/* @fn minilog_set_current_locale(void) 
 * @brief apply  current locale  (l18n & l10n) for portability 
 */
static int minilog_set_current_locale(void) ; 

//static int minilog_set_current_localv(int opt l1x ,  ... ) ; 

/* @fn  minilog(int  , const char  * , __gnu_variadic__) 
 * @brief write formated log on stdandard output with color indication 
 *        see   __log_level enum 
 * @param  int - __log_level   
 * @param const char *  formated string  
 * @param ...  variadic parameter   
 * @return   int - 0 ok  otherwise  error  
 */

__mlog int minilog(int __log_level , const char *__restrict__ __fmtstr , ... ) 
  __attribute__((format (printf ,  2,3))); 

/**
 * @fn minilog_apply_lglvl(int)
 * @brief  Apply log level with the right termcap color  
 * @param  int  -- log level 
 * */
static   __always_inline int minilog_apply_lglvl(int __log_level)  
{
  int what_happen = 0 ; 
   switch (__log_level) 
   {
     case INFO  : __get_lp_level(INFO) ; 
                  what_happen  = (MM_INFO  <<4) ; break;
     case WARN  : __get_lp_level(WARN);  
                  what_happen = (MM_WARNING<<4) ; break; 
     case ERROR : __get_lp_level(ERROR) ;  
                  what_happen = (MM_ERROR  <<4)  ; break; 
     case NOTHING: __get_lp_level(NOTHING) ;  
                  what_happen = (MM_NOSEV  <<4)   ; break; 
     /* ------------Special  log level ---------------- */
     case ALERT : 
                  __get_lp_level(ALERT) ;  
                  what_happen = (MM_ALERT <<4) | ALRT; 
                  break; 
     case FATALITY : 
                  __get_lp_level(FATALITY); 
                  what_happen = (MM_HALT <<4)  | FATL;  
                  break; 
     default : 
                what_happen=~0 /*?*/;break; 
   }

   return what_happen ; 
}

/** 
 * @fn minilog_syncom(unsigned char) 
 * @brief check  ascii character to symbolise wich type of communication
 *        user desired : e.g  : '|' symbolise the  pipe communicaion 
 *                              '=' sychronize the socket communication 
 * @param  unsigned char  -  ascii code  
 * @return the  communication  type  
 **/
static __always_inline int  minilog_syncom(unsigned char  __comtype) 
{

   /*By default  PIPE communication is used  to sychronize terminal log & logfile*/
   if(!__comtype) 
     goto  _defcom; 
  
   int magic_check = (0x7 ^ (__comtype >> 4)); 
   if(!magic_check || !magic_check ^ 4)  
       return  (__comtype ^(0xf0)) ;

_defcom: 
   return  MINILOG_COM_PIPE ; 
   
}

/** 
 * @fn minilog_exit(void)
 * @brief   basic exit function  
 * */
void  minilog_exit(void) __attribute__((noreturn)) ;

/** 
 * @fn __check_severity(int) 
 * @brief  check the severity of log 
 *    in some case when the Compile time flags MINILOG_ALLOW_ABORT_ON_FATAL  is set to > 0 
 *    this function w'll exit the application  
 * @param  int  -- severity level 
 */
static __always_inline  void  __check_severity(int __severity)  
{
  /*! Check  special severity flags */ 
  int check_special_severity = (__severity & 0xf); 
  switch(check_special_severity)
  {
     case FATL:  
#if MINILOG_ALLOW_ABORT_ON_FATAL  
       minilog_exit();  
#endif 
       /* !Nothing to do */
       break; 
  }
}

/* @fn minilog_auto_check_program_bn(void) 
 * @brief Detect the program basename 
 */
static void  minilog_auto_check_program_bn(void) 
  __attribute__((constructor)) ;

/**
 * @fn __minilog(int const char  * , ... ) 
 * @brief write formated log 
 * @param int          -   log level 
 * @param const char * -   formated string  
 * @param ...  variadic parameter   
 * @return int - 0 ok otherwise -1
 */
static int 
__minilog (int __log_level  , const char * __restrict__ __fmtstr ,   ... ) 
  __attribute__((format (printf ,  2,3))); 

/**
 * @fn __minilog_advanced(int  , struct __minilog_extended *)
 * @brief just like  __minilog function  but extended 
 *        to add more flexibility  log formating 
 *        see  file minilog_overview.c  
 * @param  int  -  log level 
 * @param  struct  __minilog_extended *  - minilog extension 
 */

static int
__minilog_advanced(int  __log_level ,  struct __minilog_extended *__restrict__ __mlog_extension) ; 


/** 
 * !status @override
 * @fn  minilog_register(int  , char * ) __user_override 
 * @brief submit  the formated log  with  combined severity level  
 * @param  int  - the severity of the log 
 * @param  char - the buffer  is aligned with  the minilog extension size   see: struct __minilog_extended
 *                But However  it can be something else  
 *
 * @return int  - the submit status 
 * */
int 
minilog_register(int __severity , char  *  __restrict__  __buffer) __user_override   ; 


/* @fn minilog_perform_locale(char  __parmreq_(1024))) 
 * @brief format time in specific representation    
 * @param  char * -  strtime_buffer with limited size 1024  null value  is no allowed 
 */

static ssize_t  
minilog_perform_locale(char strtime_buffer  __Nonullable_(1024/*1024 reserved*/)) ; 

#endif /*! __MINILOG*/
