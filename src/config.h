#ifndef CONFIG_H
#define CONFIG_H
#include "logger.h"

typedef void * CONFIG ;

CONFIG config_init_load ( void ) ;

void config_destroy ( CONFIG * phConfig ) ;

LogOptions * config_get_logOptions ( CONFIG hConfig ) ;

int config_get_port ( CONFIG hConfig ) ;

#endif
