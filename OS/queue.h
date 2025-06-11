/*!*****************************************************************************
 * file		queue.h
 * $Author: sunce.ding
 *******************************************************************************/
#ifndef _QUEUE_H_
#define _QUEUE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "OS.h"
#include "define.h"
#include "error.h"

  typedef struct QueueChain
  {
    void              *Element;   // Pointer to element.
    struct QueueChain *NextChain; // Link to next chain.
  } QueueChain_t;

  typedef struct Queue
  {
    u16           Count;
    QueueChain_t *ChainHeader;
  } Queue_t;

  void    Queue_InitQueue(void);
  void    Queue_Init(Queue_t *pQueue);
  OsErr_t Queue_Store(Queue_t *pQueue, void **ppElement);
  OsErr_t Queue_Fetch(Queue_t *pQueue, void **ppElement);
  bool    Queue_IsEmpty(Queue_t *pQueue);
  u16     Queue_GetCount(Queue_t *pQueue);

#ifdef __cplusplus
}
#endif

#endif