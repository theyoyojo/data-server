#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "util.h"
#include "logger.h"
#include "qsock.h"
#include "interrupt.h"

typedef enum exit_code { EXIT_NORMAL , EXIT_INIT_ERROR , EXIT_NET_ERROR } Exit_code ;

int main ( int argc , char * argv [] )
{
  pid_t pidMaster ;

  int arbitrary_port = 2222 ;

  // Initialize SIGINT handler
  sig_handler_init () ;
  
  // Initialize logger, load options
  // TODO load config from file
  struct log_opt log_optSession ;
  log_optSession .optEcho_priority = VERBOSE ;
  log_optSession .optLog_priority = VERBOSE ;
  log_optSession .optLog_filename = "server.log" ;
  log_optSession .optBuff_capacity = 256 ;
  log_optSession .optCreator = USER ;

  log_open ( & log_optSession ) ;

  QSOCK hServer_sock = qsock_init_bind ( arbitrary_port ) ;
  QSOCK hClient_sock ;
  qsock_listen ( hServer_sock ) ;

  pidMaster = getpid () ;
  int data ;

  while ( sigInterrupt != true )
  {
    hClient_sock = qsock_init_accept ( hServer_sock ) ; 
    if ( hClient_sock == NULL ) continue ;
    qsock_read ( hClient_sock , & data ) ;
    data *= 2 ;
    qsock_write ( hClient_sock , & data ) ;
    qsock_destroy ( & hClient_sock ) ;
    sleep ( 5 ) ;
    log_msgf ( "Server running on port %d. Press ctl-c to kill.\n" , VERBOSE , arbitrary_port) ;
  }

  qsock_destroy ( & hClient_sock ) ;
  log_close () ;
  printf ( "Exiting normally...\n" ) ;
  
  return EXIT_NORMAL  ;
}
