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

#include "d_task.h"

  void SendMsgToTask(DProcess_t *process, DMsgId_t msgId, DMsgArg_t arg);
  void RunMsgTimer(u16 counts);

#ifdef __cplusplus
}
#endif

#endif
