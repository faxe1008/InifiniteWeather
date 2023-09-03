#ifndef HEAP_4_INIFINTIME_H_
#define HEAP_4_INIFINTIME_H_


#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

void *pvPortMalloc( size_t xWantedSize );
void vPortFree( void *pv );
void vPortInitialiseBlocks( void );
void* pvPortRealloc(void* pv, size_t xWantedSize);
void vPortDeinit();

/* Stats */ 
size_t xPortGetFreeHeapSize( void );
size_t xPortGetMinimumEverFreeHeapSize( void );
size_t xPortGetLargestFreeBlock( void );

#ifdef __cplusplus
}
#endif

#endif