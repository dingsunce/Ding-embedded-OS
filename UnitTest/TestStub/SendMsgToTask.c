
#include "SendMsgToTask.h"
#include "message.h"

void SendMsgToTask(Process_t *process, MsgId_t msgId, MsgArg_t arg)
{
  Msg_SendInstant(process, msgId, arg);
  DTask_Run();
}
//-----------------------------------------------------------------------------------------------------------
void RunMsgTimer(u16 counts)
{
  while (counts)
  {
    counts--;
    Msg_RunOneTick();
    DTask_Run();
  }
}