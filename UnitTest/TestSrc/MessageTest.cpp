#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport_c.h"

#include "CleanVector.h"
#include "SendMsgToTask.h"
#include "TestReset.h"
#include "memory.h"
#include "message.h"
#include "task.h"
#include <vector>

using std::vector;

PROCESS(MessageProgress);

static vector<MsgId_t> Msgs{};
PROCESS_HANDLER(MessageProgress, msgId, arg)
{
  PROCESS_SCHEDULE_BEGIN();

  while (1)
  {

    PROCESS_WAIT_FOR_ANY_MSG();
    Msgs.push_back(msgId);
  }

  PROCESS_SCHEDULE_END();
}

TEST_GROUP(Message){TEST_SETUP(){Process_Start(&MessageProgress);
TestOutput::enableCompactFormat();
}

TEST_TEARDOWN()
{
  CleanVector(Msgs);
  TestReset();
  mock_c()->checkExpectations();
  mock_c()->clear();
  TestOutput::disableCompactFormat();
}
}
;

TEST(Message, SendInstantMessage)
{
  LONGS_EQUAL(0, Msgs.size());
  Msg_SendInstant(&MessageProgress, SYS_MSG_START_PROGRESS, NULL);
  Task_Run();
  LONGS_EQUAL(1, Msgs.size());
  LONGS_EQUAL(SYS_MSG_START_PROGRESS, Msgs[0]);
}

TEST(Message, SendMessageLater)
{
  LONGS_EQUAL(0, Msgs.size());
  Msg_SendLater(&MessageProgress, SYS_MSG_START_PROGRESS, NULL, MSG_MSEC(1000));
  RunMsgTimer(MSG_MSEC(1000));
  LONGS_EQUAL(1, Msgs.size());
  LONGS_EQUAL(SYS_MSG_START_PROGRESS, Msgs[0]);
}

TEST(Message, ReSendMessageLater)
{
  LONGS_EQUAL(0, Msgs.size());
  Msg_SendLater(&MessageProgress, SYS_MSG_START_PROGRESS, NULL, MSG_MSEC(1000));
  Msg_ReSendLater(&MessageProgress, SYS_MSG_START_PROGRESS, NULL, MSG_MSEC(1000));
  RunMsgTimer(MSG_MSEC(1000));
  RunMsgTimer(MSG_MSEC(1000));
  LONGS_EQUAL(1, Msgs.size());
  LONGS_EQUAL(SYS_MSG_START_PROGRESS, Msgs[0]);
}

TEST(Message, SendMessageCycle)
{
  LONGS_EQUAL(0, Msgs.size());
  Msg_SendCycle(&MessageProgress, SYS_MSG_START_PROGRESS, NULL, MSG_MSEC(10));
  RunMsgTimer(MSG_MSEC(10));
  RunMsgTimer(MSG_MSEC(10));
  LONGS_EQUAL(2, Msgs.size());
  LONGS_EQUAL(SYS_MSG_START_PROGRESS, Msgs[0]);
  LONGS_EQUAL(SYS_MSG_START_PROGRESS, Msgs[1]);
}

TEST(Message, SendTwoMessages)
{
  LONGS_EQUAL(0, Msgs.size());
  Msg_SendCycle(&MessageProgress, SYS_MSG_START_PROGRESS, NULL, MSG_MSEC(8));
  Msg_SendCycle(&MessageProgress, SYS_MSG_CONTINUE_PROCESS, NULL, MSG_MSEC(9));
  RunMsgTimer(MSG_MSEC(8));
  RunMsgTimer(MSG_MSEC(1));
  LONGS_EQUAL(2, Msgs.size());
  LONGS_EQUAL(SYS_MSG_START_PROGRESS, Msgs[0]);
  LONGS_EQUAL(SYS_MSG_CONTINUE_PROCESS, Msgs[1]);
}

TEST(Message, SendThreeMessages)
{
  LONGS_EQUAL(0, Msgs.size());
  Msg_SendCycle(&MessageProgress, SYS_MSG_START_PROGRESS, NULL, MSG_MSEC(8));
  Msg_SendCycle(&MessageProgress, SYS_MSG_POLL_PROCESS, NULL, MSG_MSEC(7));
  Msg_SendCycle(&MessageProgress, SYS_MSG_CONTINUE_PROCESS, NULL, MSG_MSEC(9));
  RunMsgTimer(MSG_MSEC(10));
  LONGS_EQUAL(3, Msgs.size());
  LONGS_EQUAL(SYS_MSG_POLL_PROCESS, Msgs[0]);
  LONGS_EQUAL(SYS_MSG_START_PROGRESS, Msgs[1]);
  LONGS_EQUAL(SYS_MSG_CONTINUE_PROCESS, Msgs[2]);
}

TEST(Message, CancelFirstMessage)
{
  LONGS_EQUAL(0, Msgs.size());
  Msg_SendLater(&MessageProgress, SYS_MSG_START_PROGRESS, NULL, MSG_MSEC(10));
  Msg_SendLater(&MessageProgress, SYS_MSG_START_PROGRESS, NULL, MSG_MSEC(1));

  Msg_CancelFirst(&MessageProgress, SYS_MSG_START_PROGRESS);
  RunMsgTimer(MSG_MSEC(10));

  LONGS_EQUAL(1, Msgs.size());
  LONGS_EQUAL(SYS_MSG_START_PROGRESS, Msgs[0]);
}

TEST(Message, CancelAllMessages)
{
  LONGS_EQUAL(0, Msgs.size());
  Msg_SendLater(&MessageProgress, SYS_MSG_START_PROGRESS, NULL, MSG_MSEC(10));
  Msg_SendLater(&MessageProgress, SYS_MSG_START_PROGRESS, NULL, MSG_MSEC(1));

  Msg_Cancel(&MessageProgress, SYS_MSG_START_PROGRESS);
  RunMsgTimer(MSG_MSEC(10));
  RunMsgTimer(MSG_MSEC(10));
  LONGS_EQUAL(0, Msgs.size());
}

TEST(Message, CancelCycleMessages)
{
  LONGS_EQUAL(0, Msgs.size());
  Msg_SendCycle(&MessageProgress, SYS_MSG_START_PROGRESS, NULL, MSG_MSEC(10));
  Msg_Cancel(&MessageProgress, SYS_MSG_START_PROGRESS);
  RunMsgTimer(MSG_MSEC(10));
  LONGS_EQUAL(0, Msgs.size());
}

TEST(Message, FlushMessages)
{
  LONGS_EQUAL(0, Msgs.size());
  Msg_SendLater(&MessageProgress, SYS_MSG_START_PROGRESS, NULL, MSG_MSEC(10));
  Msg_SendLater(&MessageProgress, SYS_MSG_POLL_PROCESS, NULL, MSG_MSEC(1));

  Msg_Flush(&MessageProgress);
  RunMsgTimer(MSG_MSEC(10));
  RunMsgTimer(MSG_MSEC(10));

  LONGS_EQUAL(0, Msgs.size());
}

TEST(Message, CancelAllMessagesInTaskList)
{
  LONGS_EQUAL(0, Msgs.size());
  Msg_SendInstant(&MessageProgress, SYS_MSG_START_PROGRESS, NULL);
  Msg_SendInstant(&MessageProgress, SYS_MSG_START_PROGRESS, NULL);

  Msg_Cancel(&MessageProgress, SYS_MSG_START_PROGRESS);
  RunMsgTimer(MSG_MSEC(10));

  LONGS_EQUAL(0, Msgs.size());
}

TEST(Message, FlushMessagesInTaskList)
{
  LONGS_EQUAL(0, Msgs.size());
  Msg_SendInstant(&MessageProgress, SYS_MSG_START_PROGRESS, NULL);
  Msg_SendInstant(&MessageProgress, SYS_MSG_POLL_PROCESS, NULL);

  Msg_Flush(&MessageProgress);
  RunMsgTimer(MSG_MSEC(10));

  LONGS_EQUAL(0, Msgs.size());
}

TEST(Message, FlushMessagesAndFreeResource)
{
#if (OS_ITEM_ALLOCATE_FROM_GLOBAL_MEM == 1)
  LONGS_EQUAL(0, Msgs.size());
  Msg_SendLater(&MessageProgress, SYS_MSG_START_PROGRESS, NULL, MSG_MSEC(10));

  Msg_Flush(&MessageProgress);
  RunMsgTimer(MSG_MSEC(10));

  LONGS_EQUAL(MEM_HEAP, Mem_GetFreeSize())
#endif
}

TEST(Message, FreeArgInCancellingMessages)
{
  void* arg0 = Mem_Malloc(5);
  Msg_SendLater(&MessageProgress, SYS_MSG_START_PROGRESS, arg0, MSG_MSEC(10));
  Msg_Cancel(&MessageProgress, SYS_MSG_START_PROGRESS);

  // memory of arg0 is released, it is reused by arg1
  void* arg1 = Mem_Malloc(5);
  POINTERS_EQUAL(arg1, arg0);
}

TEST(Message, FreeArgInHandlingMessages)
{
  void* arg0 = Mem_Malloc(5);
  Msg_SendLater(&MessageProgress, SYS_MSG_START_PROGRESS, arg0, MSG_MSEC(10));
  RunMsgTimer(MSG_MSEC(10));

  // memory of arg0 is released, it is reused by arg1
  void* arg1 = Mem_Malloc(5);
  POINTERS_EQUAL(arg1, arg0);
}

TEST(Message, RemianTime)
{
  LONGS_EQUAL(0, Msgs.size());
  Msg_SendLater(&MessageProgress, SYS_MSG_START_PROGRESS, NULL, MSG_MSEC(10));
  LONGS_EQUAL(10, Msg_GetRemainTime(&MessageProgress, SYS_MSG_START_PROGRESS));

  RunMsgTimer(MSG_MSEC(1));
  LONGS_EQUAL(9, Msg_GetRemainTime(&MessageProgress, SYS_MSG_START_PROGRESS));

  RunMsgTimer(MSG_MSEC(9));
  LONGS_EQUAL(0, Msg_GetRemainTime(&MessageProgress, SYS_MSG_START_PROGRESS));
}

TEST(Message, IsMessageInProcess)
{
  Msg_SendLater(&MessageProgress, SYS_MSG_START_PROGRESS, NULL, MSG_MSEC(10));
  CHECK_TRUE(Msg_IsMsgInProcess(&MessageProgress, SYS_MSG_START_PROGRESS));

  RunMsgTimer(MSG_MSEC(10));
  CHECK_FALSE(Msg_IsMsgInProcess(&MessageProgress, SYS_MSG_START_PROGRESS));
}

TEST(Message, DoNotHandleMessageWhenProcessIsIdle)
{
  Process_Exit(&MessageProgress);
  Msg_SendInstant(&MessageProgress, SYS_MSG_START_PROGRESS, NULL);
  CHECK_FALSE(Task_IsMsgInTask(&MessageProgress, SYS_MSG_START_PROGRESS));

  Msg_SendLater(&MessageProgress, SYS_MSG_START_PROGRESS, NULL, MSG_MSEC(10));
  CHECK_FALSE(Msg_IsMsgInProcess(&MessageProgress, SYS_MSG_START_PROGRESS));
}