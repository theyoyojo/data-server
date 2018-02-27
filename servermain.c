#include <stdio.h>
#include <signal.h>
#include <string.h>
#include "util.h"
#include "logger.h"
#include "qsock.h"

typedef enum exit_code { EXIT_NORMAL , EXIT_INIT_ERROR , EXIT_NET_ERROR } Exit_code ;

volatile sig_atomic_t sig_int = false ;

void sig_handler ( int sig_num )
{
  if ( sig_num == SIGINT )
  {
    sig_int = true ;
  }
}

int main ( int argc , char * argv [] )
{
  // Initialize SIGINT handler
  struct sigaction sigact ;
  memset ( & sigact , 0 , sizeof ( struct sigaction ) ) ;
  sigact .sa_handler = sig_handler ;
  sigaction ( SIGINT , & sigact , NULL ) ;

  // Initialize logger, load options
  // TODO load config from file
  struct log_opt log_optSession ;
  log_optSession .optEcho_priority = VERBOSE ;
  log_optSession .optLog_priority = VERBOSE ;
  log_optSession .optLog_filename = "server.log" ;
  log_optSession .optBuff_capacity = 256 ;
  log_optSession .optCreator = USER ;

  log_open ( & log_optSession ) ;

  QSOCK s = qsock_init_bind ( 2222 ) ;

  while ( sig_int != true )
  {
    sleep ( 5 ) ;
    log_msg ( "Server running. Press ctl-c to kill.\n" , VERBOSE ) ;
  }

  qsock_destroy ( & s ) ;
  log_close () ;
  printf ( "Exiting normally...\n" ) ;
  
  return EXIT_NORMAL  ;
}
