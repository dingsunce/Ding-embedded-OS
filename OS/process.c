/*!*****************************************************************************
 * file		process.c
 * $Author: sunce.ding
 *******************************************************************************/
#include "process.h"
#include "DList.h"
#include "memory.h"
#include "message.h"

#define PROCESS_STATE_NONE    0
#define PROCESS_STATE_RUNNING 1

DLIST(MyProcessList);

PUBLIC_PROCESS(BroadcastProcess);

PROCESS_HANDLER(BroadcastProcess, msgId, arg)
{
  PROCESS_SCHEDULE_BEGIN()

  while (1)
  {
    PROCESS_WAIT_FOR_MSG(msgId == SYS_MSG_START_PROGRESS);
  }

  PROCESS_SCHEDULE_END()
}

static volatile u8 PollRequested;
static void        Do_Poll(void);
//-----------------------------------------------------------------------------------------------------------
void Process_InitStructure(Process_t *p, ProcessHandler handler)
{
  if (p->Handler != NULL) // already Initialized Process
    return;

  p->Handler = handler;
  PT_INIT(&p->Pt);
  DList_Init(&p->ProcessList);
  DList_Init(&p->TimerList);
  p->State = PROCESS_STATE_NONE;
  p->NeedPoll = false;
}
//-----------------------------------------------------------------------------------------------------------
void Process_InitStruct(Process_t *p, ProcessHandler handler, char *name)
{
  if (p->Handler != NULL) // already Initialized Process
    return;

  Process_InitStructure(p, handler);

#if (PROCESS_DEBUG == 1)
  p->name = name;
#endif
}
//-----------------------------------------------------------------------------------------------------------
void Process_Start(Process_t *p)
{
  if (p->Handler == NULL) // it is a UnInitialized Process
    return;

  DList_t *tmp = MyProcessList.next;
  while (tmp != &MyProcessList)
  {
    Process_t *q = ContainerOf(tmp, Process_t, ProcessList);
    tmp = tmp->next;
    if (q == p)
      return; // it is already in MyProcessList
  }

  DList_Init(&p->ProcessList);
  DList_Init(&p->TimerList);
  DList_Add(&MyProcessList, &p->ProcessList);

  p->State = PROCESS_STATE_RUNNING;
  PT_INIT(&p->Pt);
  Process_HandleMsg(p, SYS_MSG_START_PROGRESS, NULL);
}
//-----------------------------------------------------------------------------------------------------------
void Process_ReStart(Process_t *p)
{
  Process_Exit(p);
  PT_INIT(&p->Pt);
  Process_Start(p);
}
//-----------------------------------------------------------------------------------------------------------
static void Process_HandleUnicastMsg(Process_t *p, MsgId_t msg, MsgArg_t data)
{
  if ((p->State & PROCESS_STATE_RUNNING) && p->Handler != NULL)
  {
    u8 ret = p->Handler(p, msg, data);
    if (ret == PT_EXITED || ret == PT_ENDED)
      Process_Exit(p);
  }
}
//-----------------------------------------------------------------------------------------------------------
static void Process_HandleBroadcastMsg(MsgId_t msg, MsgArg_t data)
{
  DList_t *tmp = MyProcessList.next;
  while (tmp != &MyProcessList)
  {
    Process_t *p = ContainerOf(tmp, Process_t, ProcessList);
    tmp = tmp->next;

    Process_HandleUnicastMsg(p, msg, data);
  }
}
//-----------------------------------------------------------------------------------------------------------
void Process_HandleMsg(Process_t *p, MsgId_t msg, MsgArg_t data)
{
  if (p == &BroadcastProcess)
    Process_HandleBroadcastMsg(msg, data);
  else
    Process_HandleUnicastMsg(p, msg, data);
}
//-----------------------------------------------------------------------------------------------------------
void Process_Exit(Process_t *p)
{
  if (Process_IsRunning(p))
  {
    p->State = PROCESS_STATE_NONE;
    DList_Remove(&p->ProcessList);
    Msg_Flush(p);
  }
}
//-----------------------------------------------------------------------------------------------------------
void Process_Init(void)
{
  DList_t *tmp = MyProcessList.next;
  while (tmp != &MyProcessList)
  {
    Process_t *p = ContainerOf(tmp, Process_t, ProcessList);
    tmp = tmp->next;
    Process_Exit(p);
  }

  DList_Init(&MyProcessList);
}
//-----------------------------------------------------------------------------------------------------------
static void Do_Poll(void)
{
  PollRequested = 0;
  DList_t *tmp = MyProcessList.next;
  while (tmp != &MyProcessList)
  {
    Process_t *p = ContainerOf(tmp, Process_t, ProcessList);
    tmp = tmp->next;

    if (p->NeedPoll)
    {
      p->State = PROCESS_STATE_RUNNING;
      p->NeedPoll = 0;
      Process_HandleMsg(p, SYS_MSG_POLL_PROCESS, NULL);
    }
  }
}
//-----------------------------------------------------------------------------------------------------------
void Process_Run(void)
{
  if (PollRequested)
    Do_Poll();
}
//-----------------------------------------------------------------------------------------------------------
void Process_Poll(Process_t *p)
{
  if (p != NULL)
  {
    if (p->State == PROCESS_STATE_RUNNING)
    {
      p->NeedPoll = 1;
      PollRequested = 1;
    }
  }
}
//-----------------------------------------------------------------------------------------------------------
bool Process_IsRunning(Process_t *p)
{
  return p->State == PROCESS_STATE_RUNNING;
}
//-----------------------------------------------------------------------------------------------------------
bool Process_IsBroadcastProcess(Process_t *p)
{
  return p == &BroadcastProcess;
}
//-----------------------------------------------------------------------------------------------------------
void *CreateProcessArg(Process_t *p)
{
  ProcessArg_t *arg = (ProcessArg_t *)Mem_Malloc(sizeof(ProcessArg_t));
  if (arg != NULL)
    arg->Process = p;

  return arg;
}
