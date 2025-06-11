/*!*****************************************************************************
 * file		memb.c(Implementation of memory block)
 * $Author: sunce.ding
 *******************************************************************************/
#include "memb.h"

#if (FAST_MEMB == 0)
//-----------------------------------------------------------------------------------------------------------
void Memb_Init(Memb_t *m)
{
  memset(m->Count, 0, m->Num);
  memset(m->Mem, 0, m->Size * m->Num);
}
//-----------------------------------------------------------------------------------------------------------
void *Memb_Alloc(Memb_t *m)
{
  u32 i;

  for (i = 0; i < m->Num; ++i)
  {
    if (m->Count[i] == 0)
    {
      ++m->Count[i];
      return (void *)((u8 *)m->Mem + (i * m->Size));
    }
  }

  return NULL;
}
//-----------------------------------------------------------------------------------------------------------
s8 Memb_Free(Memb_t *m, void *ptr)
{
  u32 i;
  u8 *ptr2 = (u8 *)m->Mem;
  for (i = 0; i < m->Num; ++i)
  {
    if (ptr2 == (u8 *)ptr)
    {
      if (m->Count[i] > 0)
        --m->Count[i];

      return m->Count[i];
    }

    ptr2 += m->Size;
  }

  return -1;
}
//-----------------------------------------------------------------------------------------------------------
bool Memb_InMem(Memb_t *m, void *ptr)
{
  u32 i;
  u8 *ptr2 = (u8 *)m->Mem;
  for (i = 0; i < m->Num; ++i)
  {
    if (m->Count[i] > 0 && ptr2 == (u8 *)ptr)
      return true;

    ptr2 += m->Size;
  }

  return false;
}
//-----------------------------------------------------------------------------------------------------------
u16 Memb_NumFree(Memb_t *m)
{
  u16 i;
  u16 numFree = 0;

  for (i = 0; i < m->Num; ++i)
  {
    if (m->Count[i] == 0)
    {
      ++numFree;
    }
  }

  return numFree;
}
//-----------------------------------------------------------------------------------------------------------
bool Memb_HasFreeNum(Memb_t *m)
{
  u32 i;

  for (i = 0; i < m->Num; ++i)
  {
    if (m->Count[i] == 0)
    {
      return true;
    }
  }

  return false;
}
#else

//-----------------------------------------------------------------------------------------------------------
void Memb_Init(Memb_t *m)
{
  SList_Init(&m->FreeList);
  memset(m->Mem, 0, m->Size * m->Num);
  for (u16 i = 0; i < m->Num; i++)
  {
    Memblock_t *block = m->block + i;
    SList_Init(&block->list);
    block->index = i;
    block->InUse = false;
    SList_Push(&m->FreeList, &block->list);
  }
}
//-----------------------------------------------------------------------------------------------------------
void *Memb_Alloc(Memb_t *m)
{
  SList_t *myList = SList_Pop(&m->FreeList);
  if (myList != NULL)
  {
    Memblock_t *block = ContainerOf(myList, Memblock_t, list);
    block->InUse = true;
    return (void *)((u8 *)m->Mem + (block->index * m->Size));
  }

  return NULL;
}
//-----------------------------------------------------------------------------------------------------------
s8 Memb_Free(Memb_t *m, void *ptr)
{
  if (ptr < m->Mem || ptr > (void *)((u8 *)m->Mem + (m->Num * m->Size)))
    return -1;

  u16 index = (u16)(((u32)ptr - (u32)m->Mem) / m->Size);
  u16 offset = (u16)(((u32)ptr - (u32)m->Mem) % m->Size);
  if (offset != 0)
    return -1;

  Memblock_t *block = m->block + index;
  if (block->InUse)
  {
    block->InUse = false;
    SList_Push(&m->FreeList, &block->list);
    return 0;
  }

  return -1;
}
//-----------------------------------------------------------------------------------------------------------
bool Memb_InMem(Memb_t *m, void *ptr)
{
  if (ptr < m->Mem || ptr > (void *)((u8 *)m->Mem + (m->Num * m->Size)))
    return false;

  u16 index = (u16)(((u32)ptr - (u32)m->Mem) / m->Size);
  u16 offset = (u16)(((u32)ptr - (u32)m->Mem) % m->Size);
  if (offset != 0)
    return false;

  Memblock_t *block = m->block + index;
  return block->InUse;
}
//-----------------------------------------------------------------------------------------------------------
u16 Memb_NumFree(Memb_t *m)
{
  u16      numFree = 0;
  SList_t *tmp = m->FreeList.next;
  while (tmp != NULL)
  {
    tmp = tmp->next;
    ++numFree;
  }

  return numFree;
}
//-----------------------------------------------------------------------------------------------------------
bool Memb_HasFreeNum(Memb_t *m)
{
  u16      numFree = 0;
  SList_t *tmp = m->FreeList.next;
  while (tmp != NULL)
  {
    tmp = tmp->next;
    ++numFree;
  }

  return numFree;
}
#endif
