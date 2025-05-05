/* @file logprint.h 
 * @brief a simple logger with termcap embedded 
 * @author  Umar Ba <jUmarB@protonmail.com> 
 */

#if !defined(__MINILOG) 
#define  __MINILOG 

//#include <stddef.h>  

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
    fmtmsg(MM_CONSOLE|MM_PRINT , "FTPFM:v1.0" , __severity_level , __mesg ,0/* no action */, 0/* no tag*/)

#define  tc_int  1, putchar  
#define  tc_exec(__termcap)  tputs(__termcap , tc_int) 
#define  tc_exec_p(__t)   tc_exec(tiparm(__t) )
//!NOTE  For cursor  position 
#define  tc_cursor_at(__x , __y)  tigoto(__x , __y) 


#define RSET_CODE 0x27 
#define FGRD_CODE 0x167 

//  ((TERMTYPE*)(cur_term))->Strings[__code] 
#define  SETAF  ((TERMTYPE *)(cur_term))->Strings[359] 
#define  RSET   ((TERMTYPE *)(cur_term))->Strings[39] 

#define  __restore  tc_exec(RSET) 
#define  tc_color_attr(__color_attribute)  SETAF , __color_attribute  

enum __log_level { 
  INFO,
#define LP_INFO  tc_color_attr(COLOR_CYAN) 
  WARN, 
#define LP_WARN  tc_color_attr(COLOR_YELLOW) 
  ERROR
#define LP_ERROR tc_color_attr(COLOR_RED) 

//!TODO  : add new log level  named FATALITY   & ALERT (WARN + BLINK) 
} ; 

#define __get_lp_level(__lp_level) tc_exec_p(LP_##__lp_level)  

#define  __LP_GENERIC(__lvl , ...) minilog(__lvl ,__VA_ARGS__)

#define  LOGINFO(...) __LP_GENERIC(INFO, __VA_ARGS__)  
#define  LOGWARN(...) __LP_GENERIC(WARN, __VA_ARGS__)  
#define  LOGERR(...)  __LP_GENERIC(ERROR,__VA_ARGS__)  

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

/* @fn __minilog(const char  * , ... ) 
 * @brief write formated log 
 * @param const char *  formated string  
 * @param ...  variadic parameter   
 * @return int - 0 ok otherwise -1
 */
static int 
__minilog (const char * __restrict__ __fmtstr ,  ...);  

/*  @fn minilog_perform_locale(char  __parmreq_(1024))) 
 *  @brief format time in specific representation  
 *  @param  char  strtime_buffer with limited size 1024  null value  is no allowed 
 */
static void 
minilog_perform_locale(char strtime_buffer  __Nonullable_(1024)) ; 

#endif /*! __MINILOG*/
