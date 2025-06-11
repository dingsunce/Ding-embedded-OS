/*!*****************************************************************************
 * file		process.h (process implementation.)
 * $Author: sunce.ding
 *******************************************************************************/
#ifndef _PROCESS_H_
#define _PROCESS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "DList.h"
#include "pt.h"

  struct Process;
  typedef u8 (*ProcessHandler)(struct Process *, MsgId_t, MsgArg_t);

  typedef struct Process
  {
    ProcessHandler Handler;
    Pt_t           Pt;
    u8             State;
    u8             NeedPoll;
    DList_t        ProcessList;
    DList_t        TimerList;
#if (PROCESS_DEBUG == 1)
    char *name;
#endif
  } Process_t;

#define PROCESS_HANDLER(name, msgId, arg)                                                                    \
  static u8(name##Handler(Process_t *process, MsgId_t msgId, MsgArg_t arg))

#if (PROCESS_DEBUG == 1)

#define PROCESS(name)                                                                                        \
  PROCESS_HANDLER(name, msgId, arg);                                                                         \
  static Process_t name = {                                                                                  \
      name##Handler, {0}, 0, 0, DLIST_INIT((name).ProcessList), DLIST_INIT((name).TimerList), #name};

#define PUBLIC_PROCESS(name)                                                                                 \
  PROCESS_HANDLER(name, msgId, arg);                                                                         \
  Process_t name = {name##Handler, {0}, 0, 0, DLIST_INIT((name).ProcessList), DLIST_INIT((name).TimerList),  \
                    #name};
#else

#define PROCESS(name)                                                                                        \
  PROCESS_HANDLER(name, msgId, arg);                                                                         \
  static Process_t name = {                                                                                  \
      name##Handler, {0}, 0, 0, DLIST_INIT((name).ProcessList), DLIST_INIT((name).TimerList)};

#define PUBLIC_PROCESS(name)                                                                                 \
  PROCESS_HANDLER(name, msgId, arg);                                                                         \
  Process_t name = {name##Handler, {0}, 0, 0, DLIST_INIT((name).ProcessList), DLIST_INIT((name).TimerList)};
#endif

#define PROCESS_SCHEDULE_BEGIN() PT_BEGIN(&process->Pt)

#define PROCESS_SCHEDULE_END() PT_END(&process->Pt)

#define PROCESS_WAIT_FOR_ANY_MSG() PT_YIELD(&process->Pt)

#define PROCESS_WAIT_FOR_MSG(c) PT_YIELD_UNTIL(&process->Pt, c)

#define PROCESS_EXIT() PT_EXIT(&process->Pt)

  typedef struct ProcessArg
  {
    void *Process;
  } ProcessArg_t;

  extern Process_t BroadcastProcess;

  void Process_Start(Process_t *p);

  void Process_ReStart(Process_t *p);

  void Process_Exit(Process_t *p);

  void Process_HandleMsg(Process_t *p, MsgId_t msg, MsgArg_t data);

  void Process_Poll(Process_t *p);

  void Process_Init(void);

  void Process_Run(void);

  bool Process_IsRunning(Process_t *p);

  void *CreateProcessArg(Process_t *p);

  void Process_InitStructure(Process_t *p, ProcessHandler handler);

  // Process_InitStruct is same as Process_InitStructure when PROCESS_DEBUG is off
  void Process_InitStruct(Process_t *p, ProcessHandler handler, char *name);

  bool Process_IsBroadcastProcess(Process_t *p);

#ifdef __cplusplus
}
#endif

#endif
