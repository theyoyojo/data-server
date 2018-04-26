#ifndef LOGGER_H
#define LOGGER_H
#include "util.h"
#include <stdarg.h>

typedef enum priority { CRITICAL , ERROR , INFO , DEBUG , VERBOSE } Priority ;

typedef struct log_opt
{
  Priority optEcho_priority ;
  Priority optLog_priority ;
  char * optLog_filename ;
  unsigned long int optBuff_capacity ;
  Author optCreator ;
} LogOptions ;

void log_open ( LogOptions * log_optSession_saved ) ;

void log_close ( void ) ;

void log_msg ( const char * msg , Priority priority ) ;

void log_msgf ( const char * fmt , Priority priority , ... ) ;

#endif 
