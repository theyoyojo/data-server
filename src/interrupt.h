#ifndef INTERRUPT_H
#define INTERRUPT_H "interrupt.h"
#include <signal.h>

volatile sig_atomic_t sigInterrupt ;

void sig_handler_init ( void ) ;

#endif
