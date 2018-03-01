#include "qsock.h"
#include "logger.h"
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>

#define DEFAULT_BACKLOG 5
#define CAST Qsock * pQsock = ( Qsock * ) hQsock 

typedef struct qsock
{
  Status state ;
  int port ;
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
  
  pQsock -> port = port ;
  pQsock -> state = SUCCESS ;

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
    log_msgf ( "Cannot socket to port %d. %s\n" , CRITICAL , port , strerror ( errno ) ) ;
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

QSOCK qsock_init_accept ( QSOCK hQsock )
{
  CAST ;
  Qsock * pClient ;

  if ( ( pClient = ( Qsock * ) malloc ( sizeof ( Qsock ) ) ) == NULL ) return NULL ;

  pClient -> fd = accept ( pQsock -> fd , & pQsock -> sa_in , sizeof ( struct sockaddr_in ) ) ;
  if ( pClient -> fd < 0 )
  {
    log_msgf ( "Failure to accept connection from a client on port %d. %s\n" ,
        ERROR ,
        pQsock -> port ,
        strerror ( errno ) ) ;
    free ( pClient ) ;
    return NULL ;
  }

  pClient -> sa_in = pQsock -> sa_in ;
  pClient -> port = pQsock -> port ;
  pClient -> state = SUCCESS ;

  log_msg ( "Accepted a new client connection\n" , DEBUG ) ;

  return ( QSOCK ) pClient ;
}

void qsock_listen ( QSOCK hQsock )
{
  CAST ;
  if ( listen ( pQsock -> fd , DEFAULT_BACKLOG ) < 0 )
  {
    log_msgf ( "Unable to listen on port %d. Is it already in use? %s\n" ,
        CRITICAL ,
        pQsock -> port ,
        strerror ( errno ) ) ;
    pQsock -> state = FAILURE ;
  }
  log_msgf ( "Listening for connections on port %d...\n" , INFO , pQsock -> port ) ;
}

void qsock_destroy ( QSOCK * phQsock )
{
  close ( ( ( Qsock * ) ( * phQsock ) ) -> fd ) ;

  free ( * phQsock ) ; * phQsock = NULL ;
}

Status qsock_get_state ( QSOCK hQsock )
{
  CAST ; return pQsock -> state ;
}

void qsock_read ( QSOCK hQsock, int * buff )
{
  CAST ;
  if ( read ( pQsock -> fd , buff , sizeof ( int ) ) < 0 )
  {
    log_msgf ( "Error reading data from socket. %s\n" , ERROR , strerror ( errno ) ) ;
  }
  log_msg ( "Successfully read data from socket" , DEBUG ) ;
}

void qsock_write ( QSOCK hQsock, int * buff )
{
  CAST ;
  if ( write ( pQsock -> fd , buff , sizeof ( int ) ) < 0 )
  {
    log_msgf ( "Error writing data to socket. %s\n" , ERROR , strerror ( errno ) ) ;
  }

  log_msg ( "Successfully wrote data to socket" , DEBUG ) ;
}
