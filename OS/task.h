/*!*****************************************************************************
 * file		task.h
 * $Author: sunce.ding
 *******************************************************************************/
#ifndef _TASK_H
#define _TASK_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "OS.h"
#include "define.h"
#include "error.h"
#include "process.h"

  void    Task_Init(void);
  OsErr_t Task_Store(Process_t *process, MsgId_t msgId, MsgArg_t arg);
  void    Task_Run(void);
  void    Task_CancelMsg(Process_t *process, MsgId_t msgId);
  void    Task_FlushMsg(Process_t *process);
  bool    Task_IsMsgInTask(Process_t *process, MsgId_t msgId);

#ifdef __cplusplus
}
#endif

#endif