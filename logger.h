#ifndef LOGGER_H
#define LOGGER_H
#include "util.h"

typedef enum priority { CRITICAL , ERROR , DEBUG , INFO , VERBOSE } Priority ;

struct log_opt
{
  Priority optEcho_priority ;
  Priority optLog_priority ;
  char * optLog_filename ;
  unsigned long int optBuff_capacity ;
  Author optCreator ;
} ;

void log_open ( struct log_opt * log_optSession_saved ) ;

void log_close ( void ) ;

void log_msg ( const char * msg , Priority priority ) ;

#endif 
