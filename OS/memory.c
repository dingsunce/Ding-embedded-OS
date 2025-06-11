/*!*****************************************************************************
 * file		memory.c
 * $Author: sunce.ding
 *******************************************************************************/
#include "memory.h"

#if (MEM_DEBUG == 1)
#define MEM_ALLOC 0
#define MEM_FREE  'F'
static u16 MemAlo = 0;
static u16 MemMaxAlo = 0;
static u16 MemFree = MEM_HEAP;
static u16 MemAloFailed = 0;
#endif

// add a null block at the end
static u8   TheHeap[MEM_HEAP + MEM_HEADER_SIZE]
#if defined PLATFORM_STM8 && defined _IAR_
    @0x180; // Avoid IAR alloc in the last position of RAM, which result in damaging stack.
#else
    ;
#endif

static MemHdr_t *FirstHeader;
static MemHdr_t *FreeHeader; // pointer to the lowest free block
static MemHdr_t *EndHeader;
//-----------------------------------------------------------------------------------------------------------
#if (MEM_DEBUG == 1)
static void Mem_DebugInit(void)
{
  memset(TheHeap, MEM_FREE, sizeof(TheHeap));
  MemAlo = 0;
  MemMaxAlo = 0;
  MemAloFailed = 0;
  MemFree = MEM_HEAP;
}
//-----------------------------------------------------------------------------------------------------------
static void Mem_DebugAllocate(MemHdr_t *hdr)
{
  MemAlo += hdr->Len;
  if (MemMaxAlo < MemAlo)
    MemMaxAlo = MemAlo;

  MemFree -= hdr->Len;
}
//-----------------------------------------------------------------------------------------------------------
static void Mem_DebugAllocateFailed(void)
{
  MemAloFailed++;
}
//-----------------------------------------------------------------------------------------------------------
static void Mem_DebugFree(MemHdr_t *hdr)
{
  MemAlo -= hdr->Len;
  MemFree += hdr->Len;
  // set data in block as MEM_FREE
  memset((u8 *)(hdr + 1), MEM_FREE, (hdr->Len - MEM_HEADER_SIZE));
}
//-----------------------------------------------------------------------------------------------------------
static void Mem_DebugFreeHeader(MemHdr_t *hdr)
{
  // set header of block as MEM_FREE
  memset((u8 *)hdr, MEM_FREE, MEM_HEADER_SIZE);
}
//-----------------------------------------------------------------------------------------------------------
u16 Mem_GetAllocSize(void)
{
  return MemAlo;
}
//-----------------------------------------------------------------------------------------------------------
u16 Mem_GetFreeSize(void)
{
  return MemFree;
}
#endif
//-----------------------------------------------------------------------------------------------------------
void Mem_Init(void)
{
#if (MEM_DEBUG == 1)
  Mem_DebugInit();
#endif

  FirstHeader = (MemHdr_t *)TheHeap;
  FirstHeader->Len = MEM_HEAP;
  FirstHeader->InUse = false;

  EndHeader = (MemHdr_t *)(&TheHeap[MEM_HEAP]);
  EndHeader->Len = 0;
  EndHeader->InUse = true;

#if (FAST_MEM == 1)
  FirstHeader->PreLen = 0;
  EndHeader->PreLen = MEM_HEAP;
#endif

  FreeHeader = FirstHeader;
}
//-----------------------------------------------------------------------------------------------------------
static void Mem_SplitBlock(MemHdr_t *hdr, u16 blockSize)
{
  u16 redundant = (u16)(hdr->Len - blockSize);
  if (redundant >= MEM_MIN_BLOCK_SIZE)
  {
    MemHdr_t *split = (MemHdr_t *)((u8 *)hdr + blockSize);
#if (FAST_MEM == 1)
    MemHdr_t *next = (MemHdr_t *)((u8 *)hdr + hdr->Len);
#endif

    split->Len = redundant;
    split->InUse = false;
    hdr->Len = blockSize;
    hdr->InUse = true;

#if (FAST_MEM == 1)
    split->PreLen = hdr->Len;
    next->PreLen = split->Len;
#endif
  }
  else
  {
    hdr->InUse = true;
  }
}
//-----------------------------------------------------------------------------------------------------------
static void UpdateFreeHeader(MemHdr_t *hdr)
{
  if (FreeHeader == hdr)
  {
    // Find next free block after hdr and update the lowest free pointer
    while (FreeHeader->InUse && FreeHeader != EndHeader)
      FreeHeader = (MemHdr_t *)((u8 *)FreeHeader + FreeHeader->Len);
  }
}
//-----------------------------------------------------------------------------------------------------------
void *Mem_Malloc(u16 size)
{
#if MEM_ALIGMENT_4BYTES
  size = ((size + 3) / 4) * 4;
#endif

  MemHdr_t *hdr = FreeHeader;
  u16       blockSize = size + MEM_HEADER_SIZE;

  if (size == 0)
    return NULL;

  while (hdr != EndHeader)
  {
    if (hdr->InUse == false && hdr->Len >= blockSize)
      break;

    hdr = (MemHdr_t *)((u8 *)hdr + hdr->Len);
  }

  if (hdr == EndHeader)
  {
#if (MEM_DEBUG == 1)
    Mem_DebugAllocateFailed();
#endif
    return NULL;
  }

  Mem_SplitBlock(hdr, blockSize);

  UpdateFreeHeader(hdr);
  memset((u8 *)(hdr + 1), 0, (hdr->Len - MEM_HEADER_SIZE));

#if (MEM_DEBUG == 1)
  Mem_DebugAllocate(hdr);
#endif

  hdr++;
  return (void *)hdr;
}
//-----------------------------------------------------------------------------------------------------------

#if (FAST_MEM == 1)
//-----------------------------------------------------------------------------------------------------------
static void Mem_CoalesceBlockFast(MemHdr_t *hdr)
{
  MemHdr_t *curHdr = hdr;
  MemHdr_t *preHdr = (MemHdr_t *)((u8 *)hdr - hdr->PreLen);
  MemHdr_t *nextHdr = (MemHdr_t *)((u8 *)hdr + hdr->Len);
  if (nextHdr != hdr && nextHdr->InUse == false)
  {
    curHdr->Len = curHdr->Len + nextHdr->Len;
#if (MEM_DEBUG == 1)
    Mem_DebugFreeHeader(nextHdr);
#endif
    nextHdr = (MemHdr_t *)((u8 *)curHdr + curHdr->Len);
    nextHdr->PreLen = curHdr->Len;
  }

  if (preHdr != hdr && preHdr->InUse == false)
  {
    preHdr->Len = preHdr->Len + curHdr->Len;
#if (MEM_DEBUG == 1)
    Mem_DebugFreeHeader(curHdr);
#endif
    nextHdr->PreLen = preHdr->Len;
  }
}
#else
//-----------------------------------------------------------------------------------------------------------
static void Mem_CoalesceBlockSlow(void)
{
  MemHdr_t *preHdr = NULL;
  MemHdr_t *hdr = FirstHeader;
#if (MEM_DEBUG == 1)
  MemHdr_t *hdrForRemove;
#endif

  while (hdr != EndHeader)
  {
    if (hdr->InUse == false)
    {
      if (preHdr != NULL && preHdr->InUse == false)
      {
        preHdr->Len += hdr->Len;
#if (MEM_DEBUG == 1)
        hdrForRemove = hdr;
#endif
        hdr = (MemHdr_t *)((u8 *)hdr + hdr->Len);
#if (MEM_DEBUG == 1)
        Mem_DebugFreeHeader(hdrForRemove);
#endif
        continue;
      }
    }

    preHdr = hdr;
    hdr = (MemHdr_t *)((u8 *)hdr + hdr->Len);
  }
}
#endif
//-----------------------------------------------------------------------------------------------------------
void Mem_Free(void *ptr)
{
  if ((u8 *)ptr < (u8 *)FirstHeader || (u8 *)ptr >= (u8 *)EndHeader)
    return;

  MemHdr_t *hdr = (MemHdr_t *)ptr - 1;
  hdr->InUse = false;

  if (FreeHeader > hdr)
    FreeHeader = hdr;

#if (MEM_DEBUG == 1)
  Mem_DebugFree(hdr);
#endif

#if (FAST_MEM == 1)
  Mem_CoalesceBlockFast(hdr);
#else
  Mem_CoalesceBlockSlow();
#endif
}
