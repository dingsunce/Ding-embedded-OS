
#include "SendMsgToTask.h"
#include "d_message.h"

void SendMsgToTask(DProcess_t *process, DMsgId_t msgId, DMsgArg_t arg)
{
  DMsg_SendInstant(process, msgId, arg);
  DTask_Run();
}
//-----------------------------------------------------------------------------------------------------------
void RunMsgTimer(u16 counts)
{
  while (counts)
  {
    counts--;
    DMsg_RunOneTick();
    DTask_Run();
  }
}