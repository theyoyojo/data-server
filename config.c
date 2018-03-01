#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#define CAST Config * pConfig = ( Config * ) hConfig

typedef struct config
{
  LogOptions * log_opt ;
  int port ;
} Config ;

CONFIG config_init_load ( void )
{
  Config * pConfig = NULL ;
  if ( ( pConfig = ( Config * ) malloc ( sizeof ( Config ) ) ) == NULL ) return NULL ;
  
  return NULL ;
}


void config_destroy ( CONFIG * phConfig )
{
  if ( ! phCOnfig ) return ;

  free ( * phConfig ) ; * phConfig = NULL 
}

LogOptions * config_get_logOptions ( CONFIG hConfig )
{
  CAST ; return pConfig -> log_opt ;

int config_get_port ( CONFIG hConfig )
{
  CAST ; return pConfig -> port ;
}
