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
#include <stdio.h>

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
    log_msgf ( "Cannot get socket from OS. %s\n" , CRITICAL , strerror ( errno ) ) ;
    pQsock -> state = FAILURE ;
  }
  else
  {
    pQsock -> state = SUCCESS ;
  }

  memset ( & pQsock -> sa_in , 0 , sizeof ( struct sockaddr_in ) ) ;
  pQsock -> sa_in .sin_family = AF_INET ;
  pQsock -> sa_in .sin_port = htons ( port ) ;
  
  pQsock -> port = port ;

  return pQsock ;
}

QSOCK qsock_init_bind ( int port )
{
  Qsock * pQsock = NULL ;
  if ( ( pQsock = qsock_init_generic ( port ) ) == NULL ) return NULL ;

  pQsock -> sa_in .sin_addr .s_addr = INADDR_ANY ;

  if ( bind ( pQsock -> fd , ( struct sockaddr * ) & pQsock -> sa_in , sizeof ( struct sockaddr_in ) ) < 0 )
  {
    log_msgf ( "Cannot bind socket to port %d. %s\n" , CRITICAL , port , strerror ( errno ) ) ;
    pQsock -> state = FAILURE  ;
  }

  return ( QSOCK ) pQsock ;
}

QSOCK qsock_init_connect ( int port , char * ip_addr )
{
  int is_validIp_addr ;
  Qsock * pQsock = NULL ;
  if ( ( pQsock = qsock_init_generic ( port ) ) == NULL ) return NULL ;

  if ( inet_aton ( ip_addr , ( struct in_addr * )  & pQsock -> sa_in .sin_addr ) < 0 )
  {
    log_msg ( "Invalid IP address. Cannot initialize socket\n" , CRITICAL ) ;
    pQsock -> state = FAILURE ;
  }

  if ( connect ( pQsock -> fd , ( struct sockaddr * ) & pQsock -> sa_in , sizeof ( struct sockaddr_in ) ) < 0 )
  {
    log_msgf ( "Failed to connect to %s:%d. %s\n" , CRITICAL , ip_addr , strerror ( errno ) ) ;
    pQsock -> state = FAILURE ;
  }

  return ( QSOCK ) pQsock ;
}

QSOCK qsock_init_accept ( QSOCK hQsock )
{
  CAST ;
  Qsock * pClient ;
  int sockaddr_size ;

  if ( ( pClient = ( Qsock * ) malloc ( sizeof ( Qsock ) ) ) == NULL ) return NULL ;

  sockaddr_size = sizeof ( struct sockaddr_in ) ;
  pClient -> fd = accept ( pQsock -> fd , ( struct sockaddr * ) & pQsock -> sa_in , & sockaddr_size ) ;
  if ( pClient -> fd < 0 )
  {
    log_msgf ( "Failure to accept connection from a client on port %d. %s\n" ,
        ERROR ,
        pQsock -> port ,
        strerror ( errno ) ) ;
    pClient -> state = FAILURE ;
  }
  else
  {
    pClient -> state = SUCCESS ;
    log_msg ( "Accepted a new client connection\n" , DEBUG ) ;
  }

  pClient -> sa_in = pQsock -> sa_in ;
  pClient -> port = pQsock -> port ;


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
  else
  {
    log_msgf ( "Listening for connections on port %d...\n" , INFO , pQsock -> port ) ;
  }
}

void qsock_destroy ( QSOCK * phQsock )
{
  if ( ! * phQsock ) return ;

  close ( ( ( Qsock * ) ( * phQsock ) ) -> fd ) ;

  free ( * phQsock ) ; * phQsock = NULL ;
}

Status qsock_get_state ( QSOCK hQsock )
{
  CAST ; return pQsock -> state ;
}

void qsock_read ( QSOCK hQsock, char * buff , int buffSize )
{
  CAST ;
  if ( read ( pQsock -> fd , buff , sizeof ( int ) ) < 0 )
  {
    log_msgf ( "Error reading %d bytes from socket. %s\n" , ERROR , buffSize , strerror ( errno ) ) ;
  }
  else log_msgf ( "Successfully read %d bytes from socket\n" , DEBUG , buffSize ) ;
}

void qsock_write ( QSOCK hQsock, char * buff, int buffSize )
{
  CAST ;
  if ( write ( pQsock -> fd , buff , sizeof ( int ) ) < 0 )
  {
    log_msgf ( "Error writing %d bytes to socket. %s\n" , ERROR , buffSize , strerror ( errno ) ) ;
  }
  else log_msgf ( "Successfully wrote %d bytes to socket\n" , DEBUG , buffSize ) ;
}
