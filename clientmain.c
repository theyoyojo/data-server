#include <stdio.h>
#include "qsock.h"

int main ( int argc , char * argv [] )
{
  QSOCK c  = NULL ;

  const char * ip_def = "localhost" ;
  int port_def = 2222 ;

  log_open ( NULL ) ;

  if ( argc >= 3 )
  {
    c = qsock_init_connect ( atoi ( argv [ 2 ] ) , ( const char * ) argv [ 1 ] ) ;
  }
  else
  {
    c = qsock_init_connect ( port_def , "localhost" ) ;
  }

  int data = 4 ;

  qsock_write ( c, & data ) ;
  qsock_read ( c , & data ) ;

  printf ( "send 4 got %d\n" , data ) ;

  qsock_destroy ( & c ) ;

  log_close () ;
  return 0 ;
}
