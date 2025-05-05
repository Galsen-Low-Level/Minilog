/* @file logprint.h 
 * @brief a simple logger with termcap embedded 
 * @author  Umar Ba <jUmarB@protonmail.com> 
 */

#if !defined(__MINILOG) 
#define  __MINILOG 

#ifndef  __parmreq_ 
# define __parmreq_(__sizereq) [static __sizereq] 
#endif 

#if defined(nptr) 
# undef nptr 
#endif  
# define nptr NULL 

#define  FLOG(__severity_level ,  __mesg) \
    fmtmsg(MM_CONSOLE|MM_PRINT , "FTPFM:v1.0" , __severity_level , __mesg ,0/* no action */, 0/* no tag*/)

#define  tc_exec(__termcap)  tputs(__termcap , 1 , putchar)  
#define  tc_exec_p(__termcaparm) tc_exec(tiparm(__termcaparm))  
#define  tc_exec_g(x,y) tputs(tigoto(cursor_address , x , y) , 1, putchar)  
#define  SETAF  ((TERMTYPE *)(cur_term))->Strings[359] 
#define  RSET   ((TERMTYPE *)(cur_term))->Strings[39] 

#define  __restore  tc_exec(RSET)  
#define  tc_color_attr(__color_attribute)   SETAF , __color_attribute 

enum __log_level { 
  INFO,
#define LP_INFO  tc_color_attr(COLOR_CYAN) 
  WARN, 
#define LP_WARN  tc_color_attr(COLOR_YELLOW) 
  ERROR
#define LP_ERROR tc_color_attr(COLOR_RED)
} ; 

#define __get_lp_level(__lp_level)\
  tc_exec_p(LP_##__lp_level) 

#define  __LP_GENERIC(__lvl , ...)\
  htftp_log(__lvl ,__VA_ARGS__)

#define  LOGINFO(...)\
  __LP_GENERIC(INFO, __VA_ARGS__)  

#define  LOGWARN(...)\
  __LP_GENERIC(WARN,__VA_ARGS__)  

#define  LOGERR(...)\
  __LP_GENERIC(ERROR,__VA_ARGS__)  

/* @fn htftp_lp_setup(void) ; 
 * @brief configure or initilize the terminal capbilities  
 */
FTPFM_LP extern int 
htftp_lp_setup(void) ;  


/* @fn  htftp_log(int  , const char  * ,  ...) 
 * @brief write formated log on stdandard output with color indication 
 *        see   __log_level enum 
 * @param  int - __log_level   
 * @param const char *  formated string  
 * @param ...  variadic parameter   
 * @return   int - 0 ok  otherwise  ~1
 *  
 */
FTPFM_LP extern int
htftp_log(int __log_level , const char *__restrict__ __fmtstr , ...) ; 
#define  htftp_lp  htftp_log

/* @fn htftp_log(const char  * , ... ) 
 * @brief write formated log 
 * @param const char *  formated string  
 * @param ...  variadic parameter   
 * @return int - 0 ok otherwise -1
 */
static int 
__htftp_log(const char * __restrict__ __fmtstr , ...);  

/*  @fn htftp_perform_localtime(char  __parmreq_(1024))) 
 *  @brief format time in specific representation  
 *  @param  char  strtime_buffer with limited size 1024  null value  is no allowed 
 */
static void 
htftp_perform_localtime(char strtime_buffer  __parmreq_(1024)) ; 

#endif /*! __MINILOG*/
