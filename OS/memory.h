/*!*****************************************************************************
 * file		memory.h
 * $Author: sunce.ding
 *******************************************************************************/
#ifndef _MEMORY_H
#define _MEMORY_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "OS.h"
#include "define.h"

  typedef struct MemHdr
  {
    unsigned Len : 15;
    unsigned InUse : 1;

#if (FAST_MEM == 1)
    unsigned PreLen : 16;
#endif
  } MemHdr_t;

#define MEM_HEADER_SIZE    sizeof(MemHdr_t)
#define MEM_MIN_BLOCK_SIZE (MEM_HEADER_SIZE * 2)

  void  Mem_Init(void);
  void *Mem_Malloc(u16 size);
  void  Mem_Free(void *ptr);

#if (MEM_DEBUG == 1)
  u16 Mem_GetAllocSize(void);
  u16 Mem_GetFreeSize(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
