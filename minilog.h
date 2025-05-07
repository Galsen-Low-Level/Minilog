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

#if defined(__cplusplus)
# define  __mlog  extern "C" 
#else
# define  __mlog 
#endif 

#define __Nullable 
#define __Nonullable_(__sizereq) [static __sizereq] 
#define __Nonullable [static  0x1] 

#if defined(__ptr_t) 
# define  nptr (__ptr_t) 0 
#else 
# define  nptr NULL
#endif


 
#define  FLOG(__severity_level ,  __mesg) \
    fmtmsg(MM_CONSOLE|MM_PRINT , ":::" , __severity_level , __mesg ,0/* no action */, 0/* no tag*/)

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
#define  tc_color_attr(__color_attribute)  mlog_exec(tiparm(SETAF , __color_attribute))   


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
#define LP_ALERT mlog_exec(BLINK); LP_WARN   
  FATALITY 
#define LP_FATALITY  mlog_exec(BOLD); LP_ERROR 
} ; 

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

/* @fn minilog_setup(void) ; 
 * @brief configure or initilize the terminal capbilities  
 */
__mlog int minilog_setup(void) ; 

/* @fn minilog_set_current_locale(void) 
 * @brief apply  current locale  (l18n & l10n) for portability 
 */
static int minilog_set_current_locale(void) ;  

/* @fn  minilog(int  , const char  * ,  ...) 
 * @brief write formated log on stdandard output with color indication 
 *        see   __log_level enum 
 * @param  int - __log_level   
 * @param const char *  formated string  
 * @param ...  variadic parameter   
 * @return   int - 0 ok  otherwise  ~1
 *  
 */
__mlog int minilog(int __log_level , const char *__restrict__ __fmtstr , ...) ; 

static   __always_inline int minilog_apply_lglvl(int __log_level)  
{
  int what_happen = 0 ; 
   switch (__log_level) 
   {
     case INFO  : __get_lp_level(INFO) ; 
                  what_happen  = (MM_INFO << 4) ; break; 
     case WARN  : __get_lp_level(WARN);  
                  what_happen = (MM_WARNING<<4) ; break; 
     case ERROR : __get_lp_level(ERROR) ;  
                  what_happen = (MM_ERROR <<4)  ; break; 
     case NOTHING: __get_lp_level(NOTHING) ;  
                  what_happen = (MM_NOSEV<<4)   ; break; 
     /* ------------Special  log level ---------------- */
     case ALERT : 
                  __get_lp_level(ALERT) ;  
                  what_happen = (MM_WARNING <<4) | 1; 
                  break; 
     case FATALITY : 
                  __get_lp_level(FATALITY); 
                  what_happen = (MM_HALT <<4) | 2; 
                  break; 
     default : 
                what_happen=~0 ;break; 
   }

   return what_happen ; 
}

static void  minilog_auto_check_program_bn(void)  __attribute__((constructor))  ; 
/* @fn __minilog(const char  * , ... ) 
 * @brief write formated log 
 * @param const char *  formated string  
 * @param ...  variadic parameter   
 * @return int - 0 ok otherwise -1
 */
static int 
__minilog (int __log_level  , const char * __restrict__ __fmtstr ,  ...);  

/*  @fn minilog_perform_locale(char  __parmreq_(1024))) 
 *  @brief format time in specific representation  
 *  @param  char  strtime_buffer with limited size 1024  null value  is no allowed 
 */
static void 
minilog_perform_locale(char strtime_buffer  __Nonullable_(1024)) ; 

#endif /*! __MINILOG*/
