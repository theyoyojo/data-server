#ifndef QSOCK_H
#define QSOCK_H "qsock.h"
#include "util.h"

struct qsock_public
{
 Status state ;
 int port ;
} ;

typedef struct qsock_public * QSOCK ;

QSOCK qsock_init_bind ( int port ) ;

QSOCK qsock_init_connect ( int port , char * ip_addr ) ;

QSOCK qsock_init_accept ( QSOCK hQsock ) ;

void qsock_destroy ( QSOCK * phQsock ) ;

void qsock_listen ( QSOCK hQsock ) ;

Status qsock_get_state ( QSOCK hQsock ) ;

void qsock_read ( QSOCK hQsock, char * buff, int buffSize ) ;

void qsock_write ( QSOCK hQsock, char * buff, int buffSize ) ;

#endif
