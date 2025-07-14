/* @file logprint.h 
 * @brief a simple logger with termcap embedded 
 * @author  Umar Ba <jUmarB@protonmail.com> 
 */

#if !defined(__MINILOG) 
#define  __MINILOG 

#include <stddef.h>  
#include <curses.h> 
#include <term.h> 
#include <fmtmsg.h> 

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
#if defined(__ptr_t) 
# define  nptr (__ptr_t) 0 
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

extern char  *minilog_basname ;  
extern int fdstream  ;  

#if defined(MINILOG_ALLOW_ABORT_ON_FATAL) 
#define MINILOG_ABORT_ON_FATALITY 1  
#endif

#define  STREAM_ON(__fstream)\
  &(struct __minilog_initial_param_t)\
  {\
    ._fstream =&(struct __minilog_record_sync){ ._record_file =__fstream}\
  }

#define INIT(...)  \
  &(struct  __minilog_initial_param_t)\
  {\
    __VA_ARGS__\
  }

enum __minilog_record_comtype  { 
   PIPE    =  0xC , 
#define   MINILOG_COM_PIPE   PIPE 
   SOCKET  
#define MINILOG_COM_SOCKET  SOCKET 
}; 

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

typedef struct   __minilog_initial_param_t mparm ; 
struct __minilog_initial_param_t { 
   struct __minilog_record_sync *  _fstream; 

}; 

typedef  void (*multi_sigcatch)(int  , ...) ;

#if !defined(DEFAULT_TARGET_SIGNALS) 
# define  DEFAULT_TARGET_SIGNALS  3,SIGINT,SIGCHLD,SIGTERM 
#endif  

#define  MLOG_DEFSIGCATCH(__nsigs , ...) \
  sigcatcher(__nsigs , ##__VA_ARGS__) 

/* @fn minilog_setup(void) ; 
 * @brief configure or initilize the terminal capbilities  
 */
__mlog int minilog_setup(struct __minilog_initial_param_t * __Nullable __initial_parameters) ; 
void minilog_cleanup(void) __attribute__((destructor));  
int  minilog_configure(struct __minilog_initial_param_t * __restrict__  __parm)  ; 
int  minilog_create_record_stream_pipeline(mr_sync * __restrict__  __source);
int  minilog_watchlog(int __fds) ; //   multi_sigcatch __variadic_signal_hanler_callback);

static int minilog_sync_pipe(const char * __restrict__  source); 

/**  
 * Can be overrided  by  using shim  technique 
 **/
__mlog void  sigcatcher(const int __nsigs ,  ...) __attribute__((weak)) ; 
__mlog void  minilog_sighdl(int __target_signal) __attribute__((weak)) ; 

static void minilog_tail_forward_sync(int __fds ) ; 

/* @fn minilog_set_current_locale(void) 
 * @brief apply  current locale  (l18n & l10n) for portability 
 */
static int minilog_set_current_locale(void) ; 
//static int minilog_set_current_localv(int opt l1x ,  ... ) ; 

/* @fn  minilog(int  , const char  * ,  ...) 
 * @brief write formated log on stdandard output with color indication 
 *        see   __log_level enum 
 * @param  int - __log_level   
 * @param const char *  formated string  
 * @param ...  variadic parameter   
 * @return   int - 0 ok  otherwise  ~1 
 */

__mlog int minilog(int __log_level , const char *__restrict__ __fmtstr , ... ) ; 

//! Apply log level with the right color  
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


void  minilog_exit(void) __attribute__((noreturn)) ;
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
 * @brief Detect the program basename  for indication
 *  
 */
static void  minilog_auto_check_program_bn(void) 
  __attribute__((constructor)) ; 
/* @fn __minilog(const char  * , ... ) 
 * @brief write formated log 
 * @param const char *  formated string  
 * @param ...  variadic parameter   
 * @return int - 0 ok otherwise -1
 */
static int 
__minilog (int __log_level  , const char * __restrict__ __fmtstr ,   ... );  

static int
__minilog_advanced(int  __log_level ,  struct __minilog_extended *__restrict__ __mlog_extension) ; 

int 
minilog_register(int __severity , char  *  __restrict__  __buffer)  ; 


/* @fn minilog_perform_locale(char  __parmreq_(1024))) 
 * @brief format time in specific representation  
 * @param  char  strtime_buffer with limited size 1024  null value  is no allowed 
 */


static ssize_t  
minilog_perform_locale(char strtime_buffer  __Nonullable_(1024 /*1024 reserved*/)) ; 

#endif /*! __MINILOG*/
