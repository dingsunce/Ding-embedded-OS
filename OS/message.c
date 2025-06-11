/*!*****************************************************************************
 * file		message.c
 * $Author: sunce.ding
 *******************************************************************************/
#include "message.h"
#include "DList.h"
#include "list.h"
#include "memb.h"
#include "memory.h"
#include "task.h"

typedef struct MsgTimer
{
  u16        MsgId;
  u32        TimePeriod;
  u32        TimeMatch;
  MsgArg_t   Arg;
  Process_t *Process;
  DList_t    ListInTimerTable;
  DList_t    ListInProcess;
} MsgTimer_t;

typedef struct MsgTableItem
{
  DList_t TimerList;
} MsgTableItem_t;

#if (OS_ITEM_ALLOCATE_FROM_GLOBAL_MEM == 0)
MEMB(MsgTimerMem, MsgTimer_t, MSG_TIMER_NUM);
#endif

#if (MESSAGE_DEBUG == 1)
static u16 MsgTimerMemAllocFailedNum = 0;
static u16 MsgTimerMemAllocCurrentNum = 0;
static u16 MsgTimerMemAllocMaxNum = 0;
#endif

static u32            TimerTick = 0;
static MsgTableItem_t MsgTimerTable[MSG_TIMER_TABLE_SIZE];
static void           Msg_FreeTimer(MsgTimer_t *timer, bool freeArg);
static void           InsertToTimerTable(MsgTimer_t *timer);
//-----------------------------------------------------------------------------------------------------------
void Msg_Init(void)
{
  TimerTick = 0;
  for (u8 i = 0; i < MSG_TIMER_TABLE_SIZE; i++)
  {
    DList_Init(&MsgTimerTable[i].TimerList);
  }

#if (OS_ITEM_ALLOCATE_FROM_GLOBAL_MEM == 0)
  Memb_Init(&MsgTimerMem);
#endif
}
//-----------------------------------------------------------------------------------------------------------
void Msg_Run1ms(void)
{
  TimerTick++;

  u8              index = TimerTick % MSG_TIMER_TABLE_SIZE;
  MsgTableItem_t *table = MsgTimerTable + index;

  DList_t *tmp = table->TimerList.next;
  while (tmp != &table->TimerList)
  {
    MsgTimer_t *timer = ContainerOf(tmp, MsgTimer_t, ListInTimerTable);
    tmp = tmp->next;

    if (TimerTick == timer->TimeMatch)
    {
      Task_Store(timer->Process, timer->MsgId, timer->Arg);
      if (timer->TimePeriod != 0)
      {
        // recalculate TimeMatch and decide which table timer it belongs to
        timer->TimeMatch = TimerTick + timer->TimePeriod;
        DList_Remove(&timer->ListInTimerTable);
        InsertToTimerTable(timer);
      }
      else
      {
        DList_Remove(&timer->ListInTimerTable);
        DList_Remove(&timer->ListInProcess);
        // argument will be released in task
        Msg_FreeTimer(timer, false);
      }

      continue;
    }

    break;
  }
}
//-----------------------------------------------------------------------------------------------------------
OsErr_t Msg_SendInstant(Process_t *process, MsgId_t msgId, MsgArg_t arg)
{
  if (!Process_IsBroadcastProcess(process)) // if it is not a broadcast process
  {
    if (process == NULL || (!Process_IsRunning(process)))
    {
      if (arg != NULL)
        Mem_Free(arg);

      return OS_ERR_NULL;
    }
  }

  return Task_Store(process, msgId, arg);
}
//-----------------------------------------------------------------------------------------------------------
static MsgTimer_t *AllocateTimer(void)
{
  MsgTimer_t *timer;
#if (OS_ITEM_ALLOCATE_FROM_GLOBAL_MEM == 0)
  timer = (MsgTimer_t *)Memb_Alloc(&MsgTimerMem);
#else
  timer = Mem_Malloc(sizeof(MsgTimer_t));
#endif

#if (MESSAGE_DEBUG == 1)
  if (timer != NULL)
  {
    MsgTimerMemAllocCurrentNum++;
    if (MsgTimerMemAllocMaxNum < MsgTimerMemAllocCurrentNum)
      MsgTimerMemAllocMaxNum = MsgTimerMemAllocCurrentNum;
  }
  else
  {
    MsgTimerMemAllocFailedNum++;
  }
#endif

  return timer;
}
//-----------------------------------------------------------------------------------------------------------
static void FreeTimer(MsgTimer_t *timer)
{
#if (OS_ITEM_ALLOCATE_FROM_GLOBAL_MEM == 0)
  Memb_Free(&MsgTimerMem, timer);
#else
  Mem_Free(timer);
#endif

#if (MESSAGE_DEBUG == 1)
  if (MsgTimerMemAllocCurrentNum > 0)
    MsgTimerMemAllocCurrentNum--;
#endif
}
//-----------------------------------------------------------------------------------------------------------
static u32 RemainTime(MsgTimer_t *timer)
{
  return timer->TimeMatch - TimerTick;
}
//-----------------------------------------------------------------------------------------------------------
static void InsertToTimerTable(MsgTimer_t *timer)
{
  u8              index = timer->TimeMatch % MSG_TIMER_TABLE_SIZE;
  MsgTableItem_t *table = MsgTimerTable + index;

  // insert before header if table is empty or all items in table have a match value smaller than
  // timer
  DList_t *ListInsert = &table->TimerList;
  DList_Init(&timer->ListInTimerTable);

  DList_t *tmp = table->TimerList.next;
  while (tmp != &table->TimerList)
  {
    MsgTimer_t *timerInTable = ContainerOf(tmp, MsgTimer_t, ListInTimerTable);
    if (RemainTime(timerInTable) > RemainTime(timer))
    {
      // insert an entry before tmp
      ListInsert = tmp;
      break;
    }

    tmp = tmp->next;
  }

  DList_InsertBefore(ListInsert, &timer->ListInTimerTable);
}
//-----------------------------------------------------------------------------------------------------------
static OsErr_t Msg_Send(Process_t *process, MsgId_t msgId, MsgArg_t arg, u32 delay, u32 period)
{
  MsgTimer_t *timer = NULL;

  if (!Process_IsBroadcastProcess(process)) // if it is not a broadcast process
  {
    if (process == NULL || (!Process_IsRunning(process)))
    {
      if (arg != NULL)
        Mem_Free(arg);

      return OS_ERR_NULL;
    }
  }

  if (delay == 0)
    return Msg_SendInstant(process, msgId, arg);

  timer = AllocateTimer();
  if (timer != NULL)
  {
    timer->MsgId = msgId;
    timer->TimePeriod = period;
    timer->TimeMatch = TimerTick + delay;
    timer->Arg = arg;
    timer->Process = process;

    DList_Init(&timer->ListInProcess);
    DList_Add(&process->TimerList, &timer->ListInProcess);

    InsertToTimerTable(timer);
    return OS_ERR_OK;
  }

  return OS_ERR_ALLOC;
}
//-----------------------------------------------------------------------------------------------------------
OsErr_t Msg_SendLater(Process_t *process, MsgId_t msgId, MsgArg_t arg, u32 delay)
{
  return Msg_Send(process, msgId, arg, delay, 0);
}
//-----------------------------------------------------------------------------------------------------------
OsErr_t Msg_ReSendLater(Process_t *process, MsgId_t msgId, MsgArg_t arg, u32 delay)
{
  Msg_CancelAll(process, msgId);
  return Msg_Send(process, msgId, arg, delay, 0);
}
//-----------------------------------------------------------------------------------------------------------
OsErr_t Msg_SendCycle(Process_t *process, MsgId_t msgId, MsgArg_t arg, u32 period)
{
  return Msg_Send(process, msgId, arg, period, period);
}
//-----------------------------------------------------------------------------------------------------------
OsErr_t Msg_ReSendCycle(Process_t *process, MsgId_t msgId, MsgArg_t arg, u32 period)
{
  Msg_CancelAll(process, msgId);
  return Msg_Send(process, msgId, arg, period, period);
}
//-----------------------------------------------------------------------------------------------------------
void Msg_CancelFirst(Process_t *process, MsgId_t msgId)
{
  if (process == NULL)
    return;

  DList_t *tmp = process->TimerList.next;
  while (tmp != &process->TimerList)
  {
    MsgTimer_t *timer = ContainerOf(tmp, MsgTimer_t, ListInProcess);
    tmp = tmp->next;

    if (timer->MsgId == msgId)
    {
      DList_Remove(&timer->ListInProcess);
      DList_Remove(&timer->ListInTimerTable);
      Msg_FreeTimer(timer, true);
      break;
    }
  }
}
//-----------------------------------------------------------------------------------------------------------
void Msg_Cancel(Process_t *process, MsgId_t msgId)
{
  Msg_CancelAll(process, msgId);
}
//-----------------------------------------------------------------------------------------------------------
void Msg_CancelAll(Process_t *process, MsgId_t msgId)
{
  if (process == NULL)
    return;

  DList_t *tmp = process->TimerList.next;
  while (tmp != &process->TimerList)
  {
    MsgTimer_t *timer = ContainerOf(tmp, MsgTimer_t, ListInProcess);
    tmp = tmp->next;

    if (timer->MsgId == msgId)
    {
      DList_Remove(&timer->ListInProcess);
      DList_Remove(&timer->ListInTimerTable);
      Msg_FreeTimer(timer, true);
    }
  }

  Task_CancelMsg(process, msgId);
}
//-----------------------------------------------------------------------------------------------------------
void Msg_Flush(Process_t *process)
{
  if (process == NULL)
    return;

  DList_t *tmp = process->TimerList.next;
  while (tmp != &process->TimerList)
  {
    MsgTimer_t *timer = ContainerOf(tmp, MsgTimer_t, ListInProcess);
    tmp = tmp->next;

    DList_Remove(&timer->ListInProcess);
    DList_Remove(&timer->ListInTimerTable);
    Msg_FreeTimer(timer, true);
  }

  Task_FlushMsg(process);
}
//-----------------------------------------------------------------------------------------------------------
static void Msg_FreeTimer(MsgTimer_t *timer, bool freeArg)
{
  if (timer == NULL)
    return;

  if (freeArg && timer->Arg != NULL)
    Mem_Free(timer->Arg);

  FreeTimer(timer);
}
//-----------------------------------------------------------------------------------------------------------
u32 Msg_GetRemainTime(Process_t *process, MsgId_t msgId)
{
  if (process == NULL)
    return 0;

  DList_t *tmp = process->TimerList.next;
  while (tmp != &process->TimerList)
  {
    MsgTimer_t *timer = ContainerOf(tmp, MsgTimer_t, ListInProcess);
    tmp = tmp->next;

    if (timer->MsgId == msgId)
      return timer->TimeMatch - TimerTick;
  }

  return 0;
}
//-----------------------------------------------------------------------------------------------------------
bool Msg_IsMsgInProcess(Process_t *process, MsgId_t msgId)
{
  if (process == NULL)
    return false;

  DList_t *tmp = process->TimerList.next;
  while (tmp != &process->TimerList)
  {
    MsgTimer_t *timer = ContainerOf(tmp, MsgTimer_t, ListInProcess);
    tmp = tmp->next;

    if (timer->MsgId == msgId)
      return true;
  }

  return false;
}