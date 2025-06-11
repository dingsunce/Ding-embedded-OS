/*!*****************************************************************************
 * $Author: sunce.ding
 * Copyright (c) 2015 B.E.G.
 *******************************************************************************/
#ifndef SEND_MASG_TO_TASK_H
#define SEND_MASG_TO_TASK_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "task.h"

  void SendMsgToTask(Process_t* process, MsgId_t msgId, MsgArg_t arg);
  void RunMsgTimer(u16 counts);

#ifdef __cplusplus
}
#endif

#endif
