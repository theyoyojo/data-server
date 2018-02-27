#ifndef QSOCK_H
#define QSOCK_H "qsock.h"

struct qsock_public
{
 int port ;
} ;

typedef struct qsock_public * QSOCK ;

QSOCK qsock_init_bind ( int port ) ;

QSOCK qsock_init_connect ( int port , char * ip_addr ) ;

void qsock_destroy ( QSOCK * phQsock ) ;

#endif
