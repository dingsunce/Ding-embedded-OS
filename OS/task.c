/*!*****************************************************************************
 * file		task.c
 * $Author: sunce.ding
 *******************************************************************************/
#include "task.h"
#include "list.h"
#include "memb.h"
#include "memory.h"
#include "message.h"
#include "process.h"

// The task queue.
typedef struct TaskItems
{
  LIST_HEADER;
  Process_t* Process;
  MsgId_t MsgId;
  MsgArg_t Arg;
} TaskItem_t;

LIST(TaskElementList);

#if (OS_ITEM_ALLOCATE_FROM_GLOBAL_MEM == 0)
MEMB(TaskElementMem, TaskItem_t, TASK_ITEM_NUM);
#endif

#if (TASK_DEBUG == 1)
static u16 TaskMemAllocFailedNum = 0;
static u16 TaskMemAllocCurrentNum = 0;
static u16 TaskMemAllocMaxNum = 0;
#endif

//-----------------------------------------------------------------------------------------------------------
void Task_Init(void)
{
  List_Init(TaskElementList);
#if (OS_ITEM_ALLOCATE_FROM_GLOBAL_MEM == 0)
  Memb_Init(&TaskElementMem);
#endif
}
//-----------------------------------------------------------------------------------------------------------
static TaskItem_t* AllocateElement(void)
{
#if (OS_ITEM_ALLOCATE_FROM_GLOBAL_MEM == 0)
  TaskItem_t* e = (TaskItem_t*)Memb_Alloc(&TaskElementMem);
#else
  TaskItem_t* e = Mem_Malloc(sizeof(TaskItem_t));
#endif
  if (e != NULL)
  {
#if (TASK_DEBUG == 1)
    TaskMemAllocCurrentNum++;
    if (TaskMemAllocMaxNum < TaskMemAllocCurrentNum)
      TaskMemAllocMaxNum = TaskMemAllocCurrentNum;
#endif
  }
  else
  {
#if (TASK_DEBUG == 1)
    TaskMemAllocFailedNum++;
#endif
  }

  return e;
}
//-----------------------------------------------------------------------------------------------------------
OsErr_t Task_Store(Process_t* process, MsgId_t msgId, MsgArg_t arg)
{
  TaskItem_t* e = AllocateElement();
  if (e != NULL)
  {
    e->Process = process;
    e->MsgId = msgId;
    e->Arg = arg;
    List_Add(TaskElementList, e);
    return OS_ERR_OK;
  }

  return OS_ERR_ALLOC;
}
//-----------------------------------------------------------------------------------------------------------
static void FreeElement(TaskItem_t* e)
{
#if (OS_ITEM_ALLOCATE_FROM_GLOBAL_MEM == 0)
  Memb_Free(&TaskElementMem, e);
#else
  Mem_Free(e);
#endif

#if (TASK_DEBUG == 1)
  TaskMemAllocCurrentNum--;
#endif
}
//-----------------------------------------------------------------------------------------------------------
void Task_Run(void)
{
  TaskItem_t* e = List_Pop(TaskElementList);
  if (e != NULL)
  {
    if (e->Process != NULL && e->MsgId != SYS_MSG_NONE)
      Process_HandleMsg(e->Process, e->MsgId, e->Arg);

    if (e->Arg != NULL)
      Mem_Free(e->Arg);

    FreeElement(e);
  }
}
//-----------------------------------------------------------------------------------------------------------
void Task_CancelMsg(Process_t* process, MsgId_t msgId)
{
  TaskItem_t* e;
  for (e = (TaskItem_t*)List_Head(TaskElementList); e != NULL; e = (TaskItem_t*)List_ItemNext(e))
  {
    if (e->Process == process && e->MsgId == msgId)
      e->MsgId = SYS_MSG_NONE;
  }
}
//-----------------------------------------------------------------------------------------------------------
bool Task_IsMsgInTask(Process_t* process, MsgId_t msgId)
{
  TaskItem_t* e;
  for (e = (TaskItem_t*)List_Head(TaskElementList); e != NULL; e = (TaskItem_t*)List_ItemNext(e))
  {
    if (e->Process == process && e->MsgId == msgId)
      return true;
  }

  return false;
}
//-----------------------------------------------------------------------------------------------------------
void Task_FlushMsg(Process_t* process)
{
  TaskItem_t* e;
  for (e = (TaskItem_t*)List_Head(TaskElementList); e != NULL; e = (TaskItem_t*)List_ItemNext(e))
  {
    if (e->Process == process)
      e->MsgId = SYS_MSG_NONE;
  }
}