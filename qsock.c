#include "qsock.h"
#include "logger.h"
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

typedef struct qsock
{
  int fd ;
  struct sockaddr_in sa_in ;
} Qsock ;

// Private 
Qsock * qsock_init_generic ( int port )
{
  Qsock * pQsock = NULL ;
  if ( ( pQsock = ( Qsock * ) malloc ( sizeof ( Qsock ) ) ) == NULL ) return NULL ;

  if ( ( pQsock -> fd = socket ( AF_INET , SOCK_STREAM , IPPROTO_TCP ) ) < 0 )
  {
    log_msg ( "Cannot get socket from OS\n" , CRITICAL ) ;
    log_msg ( strerror(errno) , ERROR ) ;
    free ( pQsock ) ;
    return NULL ;
  }

  memset ( & pQsock -> sa_in , 0 , sizeof ( struct sockaddr_in ) ) ;
  pQsock -> sa_in .sin_family = AF_INET ;
  pQsock -> sa_in .sin_port = htons ( port ) ;

  return pQsock ;
}

QSOCK qsock_init_bind ( int port )
{
  Qsock * pQsock = NULL ;
  if ( ( pQsock = qsock_init_generic ( port ) ) == NULL ) return NULL ;

  pQsock -> sa_in .sin_addr .s_addr = INADDR_ANY ;

  if ( bind ( pQsock -> fd , & pQsock -> sa_in , sizeof ( struct sockaddr_in ) ) < 0 )
  {
    // TODO add ability to send the port number to log_msg
    log_msg ( "Cannot socket to port \n" , CRITICAL ) ;
    log_msg ( strerror ( errno ) , ERROR ) ;
    free ( pQsock ) ;
    return NULL ;
  }

  return ( QSOCK ) pQsock ;
}

QSOCK qsock_init_connect ( int port , char * ip_addr )
{
  int is_validIp_addr ;
  Qsock * pQsock = NULL ;
  if ( ( pQsock = qsock_init_generic ( port ) ) == NULL ) return NULL ;
  if ( inet_aton ( ip_addr , & pQsock ->  sa_in .sin_addr ) < 0 )
  {
    log_msg ( "Invalid IP address coversion. Cannot initialize socket\n" , CRITICAL ) ;
    free ( pQsock ) ;
    return NULL ;
  }

  if ( connect ( pQsock -> fd ,  & pQsock -> sa_in , sizeof ( struct sockaddr_in ) ) < 0 )
  {
    log_msg ( "Failed to connect to server\n" , CRITICAL ) ;
    log_msg ( ip_addr , INFO ) ;
    log_msg ( strerror ( errno ) , INFO ) ;
    free ( pQsock ) ;
    return NULL ;
  }

  return ( QSOCK ) pQsock ;
}

void qsock_destroy ( QSOCK * phQsock )
{
  close ( ( ( Qsock * ) ( * phQsock ) ) -> fd ) ;

  free ( * phQsock ) ; * phQsock = NULL ;
}
