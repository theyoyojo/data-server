#include "logger.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DEFAULT_LOG_PRIORITY ERROR
#define DEFAULT_ECHO_PRIORITY ERROR
#define DEFAULT_LOG_FILENAME "exec.log"
#define DEFAULT_BUFFER_CAPACITY 256

char * strLog_filename_default ;

struct log_opt * log_optSession ;

char * strBuff ;
FILE * fpLog ;

char * s_critical ;
char * s_error ;
char * s_info ;
char * s_debug ;
char * s_verbose ;


void log_open ( struct log_opt * log_optSession_saved )
{
  strLog_filename_default = DEFAULT_LOG_FILENAME ;

  s_critical = "CRITICAL" ;
  s_error = "ERROR" ;
  s_info = "INFO" ;
  s_debug = "DEBUG" ;
  s_verbose = "VERBOSE" ;

  if ( log_optSession_saved )
  {
    log_optSession = log_optSession_saved ;

    if ( log_optSession -> optBuff_capacity < 1 )
    {
      fprintf ( stderr , "ERROR, cannot initalize log: buffer size "
          "must be integer greater than 0 \n" ) ; exit ( 1 ) ;
    }
  }
  else 
  {
    log_optSession = NULL ;
    if ( ( log_optSession = ( struct log_opt * ) malloc (
            sizeof ( struct log_opt ) )
         ) == NULL ) fprintf ( stderr , "ERROR, cannot initalize log "
         "with default options.\nKilling self...\n" ) , exit ( 1 ) ;

    log_optSession -> optLog_priority = DEFAULT_LOG_PRIORITY ;
    log_optSession -> optEcho_priority = DEFAULT_ECHO_PRIORITY ;
    log_optSession -> optLog_filename = strLog_filename_default ;
    log_optSession -> optBuff_capacity = DEFAULT_BUFFER_CAPACITY ;
    log_optSession -> optCreator = DEV ;

  }

  strBuff = NULL ;
  if ( ( strBuff = ( char * ) malloc (
          sizeof ( char ) * log_optSession -> optBuff_capacity )
        ) == NULL ) fprintf ( stderr , "ERROR, cannot initialize log."
        "\nKilling self..\n" ) , exit ( 1 ) ;
  memset ( strBuff , 0 , sizeof ( char ) * log_optSession -> optBuff_capacity ) ;
    
  fpLog = NULL ;
  if ( ! ( fpLog = fopen ( log_optSession -> optLog_filename , "a" ) ) )
  {
    fprintf ( stderr ,"ERROR, cannot open logfile \"%s\" for writing."
        "\nKilling self...\n" , log_optSession -> optLog_filename ) ;
    exit ( 1 ) ;
  }

}

// Return 1 if character was successfully read or 0 if it was the null terminator
// Return -1 if there is a problem with the read
int needle_read ( char * * needle , char * next )
{
  if ( ! needle || ! next ) return -1 ;

  * next = * * needle ;
  ( * needle ) ++ ;

  if ( * next == '\0' ) return 0 ;
  else                  return 1 ;
}

void strBuff_publish ( int * p_sizeBuff , Priority priority )
{
  if ( priority <= log_optSession -> optEcho_priority )
  {
    fprintf ( stdout , "%s" , strBuff ) ;
  }
  if ( priority <= log_optSession -> optLog_priority )
  {
    fprintf ( fpLog , "%s" , strBuff ) ;
  }
  * p_sizeBuff = 0 ;
  memset ( strBuff , 0 , sizeof ( char ) * log_optSession -> optLog_priority ) ; 

  return ;
}

void strBuff_append ( int * p_sizeBuff , char c , Priority priority )
{
  if ( * p_sizeBuff >= log_optSession -> optBuff_capacity )
  {
    strBuff_publish ( p_sizeBuff , priority ) ;
  }
  
  strBuff [ ( * p_sizeBuff ) ++ ] = c ;
}

void strBuff_copy_priority ( int * p_sizeBuff , Priority priority )
{
  char * needlePriority ;
  char c ;

  switch ( priority )
  {
    case CRITICAL :
      needlePriority = s_critical ;
      break ;
    case ERROR :
      needlePriority = s_error ;
      break ;
    case INFO :
      needlePriority = s_info ;
      break ;
    case DEBUG :
      needlePriority = s_debug ;
      break ;
    case VERBOSE :
      needlePriority = s_verbose ;
      break ;
    default :
      needlePriority = NULL ;
      break ;
  }

  while ( needle_read ( & needlePriority , & c ) == 1 )
  {
    strBuff_append ( p_sizeBuff , c , priority ) ;
  }

}

void strBuff_copy_integer ( int * p_sizeBuff , int n , Priority priority )
{
  if ( n > 0 )
  {
    // Integer value of char + 48 will give char representation of int
    strBuff_copy_integer ( p_sizeBuff , n / 10 , priority ) ;
    strBuff_append ( p_sizeBuff , ( char ) ( ( n % 10 ) + 48 ) , priority ) ;
  }
}

void strBuff_copy_timestamp ( int * p_sizeBuff , Priority priority )
{
  struct tm * tmNow ;
  time_t timerNow ;
  timerNow = time ( 0 ) ;
  tmNow = localtime ( & timerNow ) ; 

  strBuff_copy_integer ( p_sizeBuff , tmNow -> tm_hour , priority ) ;
  strBuff_append ( p_sizeBuff , ':' , priority ) ;
  strBuff_copy_integer ( p_sizeBuff , tmNow -> tm_min , priority ) ;
  strBuff_append ( p_sizeBuff , ':' , priority ) ;
  if                                  ( tmNow -> tm_sec < 10 )
  strBuff_append ( p_sizeBuff , '0' , priority ) ;
  if                                  ( tmNow -> tm_sec < 1 )
  strBuff_append ( p_sizeBuff , '0' , priority ) ;
  strBuff_copy_integer ( p_sizeBuff , tmNow -> tm_sec , priority ) ;
  strBuff_append ( p_sizeBuff , ' ' , priority ) ;
  strBuff_copy_integer ( p_sizeBuff , tmNow -> tm_mday , priority ) ;
  strBuff_append ( p_sizeBuff , '.' , priority ) ;
  strBuff_copy_integer ( p_sizeBuff , tmNow -> tm_mon + 1 , priority ) ;
  strBuff_append ( p_sizeBuff , '.' , priority ) ;
  strBuff_copy_integer ( p_sizeBuff , tmNow -> tm_year + 1900 , priority ) ;
}

void log_msg ( const char * msg , Priority priority ) 
{
  char * needleMsg , c ;
  int sizeBuff ;

  // If neither log nor echo is interested in the message, quit before doing anything
  if ( priority > log_optSession -> optLog_priority &&
      priority > log_optSession -> optEcho_priority ) return ;

  memset ( strBuff , 0 , sizeof ( char ) * log_optSession -> optBuff_capacity ) ;

  needleMsg = ( char * ) msg ;

  sizeBuff = 0 ;
  strBuff_append ( & sizeBuff , '[' , priority ) ;
  strBuff_copy_priority ( & sizeBuff , priority ) ;
  strBuff_append ( & sizeBuff , ' ' , priority ) ;
  strBuff_copy_timestamp ( & sizeBuff , priority ) ;
  strBuff_append ( & sizeBuff , ']' , priority ) ;
  strBuff_append ( & sizeBuff , ' ' , priority ) ;

  if ( msg )
  {
    while ( needle_read ( & needleMsg , & c ) == 1 )
    {
      strBuff_append ( & sizeBuff , c , priority ) ; 
    }
  }

  if ( sizeBuff > 0 )
  {
    strBuff_publish ( & sizeBuff , priority ) ;
  }
}

void log_msgf ( const char * fmt , Priority priority , ... )
{
  va_list argsMsg ;
  int sizeFmt ;
  char * buffMsg ;

  sizeFmt = 0 ;
  while ( fmt [ sizeFmt ++ ] != '\0' ) ;
  
  // TODO what is the best way do determine number of bytes to allocate?
  // What appears in the following line ( sizeFmt * 2 ) is a temporary fix,
  // though it may prove to be permanently sufficent 
  buffMsg = ( char * ) malloc ( sizeof ( char ) * ( sizeFmt * 2 ) ) ;

  va_start ( argsMsg , fmt ) ;
  vsprintf ( buffMsg , fmt , argsMsg ) ;
  va_end ( argsMsg ) ;

  log_msg ( ( const char * ) buffMsg , priority ) ;
  free ( buffMsg ) ;
}
  

void log_close ( void ) 
{
  fclose ( fpLog ) ;
  free ( strBuff ) ;
  
  if ( log_optSession -> optCreator == DEV )
  {
    free ( log_optSession ) ;
  }

  return ;
}
