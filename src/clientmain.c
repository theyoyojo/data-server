#include <stdio.h>
#include "qsock.h"

void putchar_binary ( char c )
{
  for ( int i = 0 ; i < 8 ; i ++ )
  {
    putchar( ( c | ( 1 << i ) ) > c ? '0' : '1' ) ;
  } 
}


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

  int req ;

  printf ( "Enter request number:" ) ;
  scanf ( "%d" , & req ) ;

  char dataBuff [ 256 ] ;
  memset ( dataBuff , 0 , 256 ) ;

  qsock_write ( c, ( char * ) & req , sizeof ( int ) ) ;
  qsock_read ( c , dataBuff , 256 ) ;

  printf ( "SERVER RESPOSE FOLLOWS:\n" ) ;
  int i ;
  for (i = 0 ; i < 256 ; i ++ )
  {
    printf ( "%d: " , i ) ;
    putchar_binary ( dataBuff [ i ] ) ;
    printf ( "\n" ) ;
  }

  qsock_destroy ( & c ) ;

  log_close () ;
  return 0 ;
}
