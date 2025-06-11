/*!*****************************************************************************
 * file		queue.c
 * $Author: sunce.ding
 *******************************************************************************/
#include "queue.h"
#include "memb.h"
#include "memory.h"

#if (OS_ITEM_ALLOCATE_FROM_GLOBAL_MEM == 0)
MEMB(QueueMem, QueueChain_t, QUEUE_ITEM_NUM);
#endif

#if (QUEUE_DEBUG == 1)
static u16 QueueMemAllocFailedNum = 0;
static u16 QueueMemAllocCurrentNum = 0;
static u16 QueueMemAllocMaxNum = 0;
#endif

static void Queue_InsertChain(Queue_t* pQueue, QueueChain_t* queueChain);
//-----------------------------------------------------------------------------------------------------------
void Queue_InitQueue(void)
{
#if (OS_ITEM_ALLOCATE_FROM_GLOBAL_MEM == 0)
  Memb_Init(&QueueMem);
#endif
}
//-----------------------------------------------------------------------------------------------------------
void Queue_Init(Queue_t* pQueue)
{
  pQueue->Count = 0;
  pQueue->ChainHeader = NULL;
}
//-----------------------------------------------------------------------------------------------------------
static QueueChain_t* AllocateChain(void)
{
#if (OS_ITEM_ALLOCATE_FROM_GLOBAL_MEM == 0)
  QueueChain_t* chain = (QueueChain_t*)Memb_Alloc(&QueueMem);
#else
  QueueChain_t* chain = Mem_Malloc(sizeof(QueueChain_t));
#endif

  if (chain != NULL)
  {
#if (QUEUE_DEBUG == 1)
    QueueMemAllocCurrentNum++;
    if (QueueMemAllocMaxNum < QueueMemAllocCurrentNum)
      QueueMemAllocMaxNum = QueueMemAllocCurrentNum;
#endif
  }
  else
  {
#if (QUEUE_DEBUG == 1)
    QueueMemAllocFailedNum++;
#endif
  }

  return chain;
}
//-----------------------------------------------------------------------------------------------------------
OsErr_t Queue_Store(Queue_t* pQueue, void** ppElement)
{
  QueueChain_t* chain = AllocateChain();
  if (chain != NULL)
  {
    pQueue->Count++;
    chain->Element = *ppElement;
    Queue_InsertChain(pQueue, chain);
    return OS_ERR_OK;
  }

  return OS_ERR_ALLOC;
}
//-----------------------------------------------------------------------------------------------------------
static void FreeChain(QueueChain_t* chain)
{
#if (OS_ITEM_ALLOCATE_FROM_GLOBAL_MEM == 0)
  Memb_Free(&QueueMem, chain);
#else
  Mem_Free(chain);
#endif

#if (QUEUE_DEBUG == 1)
  QueueMemAllocCurrentNum--;
#endif
}
//-----------------------------------------------------------------------------------------------------------
OsErr_t Queue_Fetch(Queue_t* pQueue, void** ppElement)
{
  QueueChain_t* chain = pQueue->ChainHeader;

  if (chain != NULL)
  {
    pQueue->Count--;

    pQueue->ChainHeader = chain->NextChain;

    // Fetch the element.
    *ppElement = chain->Element;

    FreeChain(chain);

    return OS_ERR_OK;
  }

  return OS_ERR_QEMPTY;
}
//-----------------------------------------------------------------------------------------------------------
bool Queue_IsEmpty(Queue_t* pQueue)
{
  return (pQueue->Count == 0);
}
//-----------------------------------------------------------------------------------------------------------
u16 Queue_GetCount(Queue_t* pQueue)
{
  return pQueue->Count;
}
//-----------------------------------------------------------------------------------------------------------
static void Queue_InsertChain(Queue_t* pQueue, QueueChain_t* queueChain)
{
  QueueChain_t* chain = pQueue->ChainHeader;

  if (chain == NULL) // Chain is empty
  {
    queueChain->NextChain = NULL;
    pQueue->ChainHeader = queueChain;
  }
  else
  {
    while (chain->NextChain != NULL)
    {
      chain = chain->NextChain;
    }
    queueChain->NextChain = NULL;
    chain->NextChain = queueChain;
  }
}