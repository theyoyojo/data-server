#include "interrupt.h"
#include <stdlib.h>
#include "util.h"

void sig_handler ( int sign )
{
  if ( sign == SIGINT )
  {
    sigInterrupt = true ;
  } 
}

void sig_handler_init ( void )
{
  struct sigaction sig_act ;
  memset ( & sig_act , 0 , sizeof ( struct sigaction ) ) ;
  sig_act .sa_handler = sig_handler ;
  sigaction ( SIGINT , & sig_act , NULL ) ; 
}

