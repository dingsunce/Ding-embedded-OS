/*!*****************************************************************************
 * file		OS.h
 * $Author: sunce.ding
 *******************************************************************************/

#ifndef OS_H_
#define OS_H_

#ifdef __cplusplus
extern "C"
{
#endif

  void OS_Init(void);

#define ContainerOf(ptr, type, member) ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))

#define OS_VERSION 0x20 // v2.0

#ifndef MEM_HEAP
#define MEM_HEAP 1500
#endif

#if (MEM_HEAP & 0x80000)
#error MAXMEMHEAP is too big to manage!
#endif

#ifndef FAST_MEM
#define FAST_MEM 0
#endif

#ifndef MEM_ALIGMENT_4BYTES
#define MEM_ALIGMENT_4BYTES 1
#endif

#ifndef FAST_MEMB
#define FAST_MEMB 0
#endif

// MSG_TIMER_TABLE_SIZE is recommended to be a prime
#ifndef MSG_TIMER_TABLE_SIZE
#define MSG_TIMER_TABLE_SIZE 7
#endif

#ifndef OS_ITEM_ALLOCATE_FROM_GLOBAL_MEM
#define OS_ITEM_ALLOCATE_FROM_GLOBAL_MEM 0
#endif

#if (OS_ITEM_ALLOCATE_FROM_GLOBAL_MEM == 0)

#ifndef MSG_TIMER_NUM
#define MSG_TIMER_NUM 100
#endif

#ifndef QUEUE_ITEM_NUM
#define QUEUE_ITEM_NUM 100
#endif

#ifndef TASK_ITEM_NUM
#define TASK_ITEM_NUM 50
#endif

#endif

#ifndef MESSAGE_DEBUG
#define MESSAGE_DEBUG 0
#endif

// Caution: If MEM_DEBUG is opened, Mem_Free will change the object contents to MEM_FREE('F')
// Do not use the object after it is freed
#ifndef MEM_DEBUG
#define MEM_DEBUG 0
#endif

#ifndef TASK_DEBUG
#define TASK_DEBUG 0
#endif

#ifndef QUEUE_DEBUG
#define QUEUE_DEBUG 0
#endif

#ifndef PROCESS_DEBUG
#define PROCESS_DEBUG 0
#endif

#ifndef SYS_TICK_REAL_TIME_CALLBACK
#define SYS_TICK_REAL_TIME_CALLBACK 0
#endif

#ifdef __cplusplus
}
#endif

#endif
