#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport_c.h"

#include "CleanVector.h"
#include "SendMsgToTask.h"
#include "TestReset.h"
#include "d_mem.h"
#include "d_message.h"
#include "d_task.h"
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
  DMsg_SendInstant(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL);
  DTask_Run();
  LONGS_EQUAL(1, Msgs.size());
  LONGS_EQUAL(SYS_MSG_TEST_PROCESS, Msgs[0]);
}

TEST(Message, SendMessageLater)
{
  LONGS_EQUAL(0, Msgs.size());
  DMsg_SendLater(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL, MSG_MSEC(1000));
  RunMsgTimer(MSG_MSEC(1000));
  LONGS_EQUAL(1, Msgs.size());
  LONGS_EQUAL(SYS_MSG_TEST_PROCESS, Msgs[0]);
}

TEST(Message, ReSendMessageLater)
{
  LONGS_EQUAL(0, Msgs.size());
  DMsg_SendLater(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL, MSG_MSEC(1000));
  DMsg_ReSendLater(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL, MSG_MSEC(1000));
  RunMsgTimer(MSG_MSEC(1000));
  RunMsgTimer(MSG_MSEC(1000));
  LONGS_EQUAL(1, Msgs.size());
  LONGS_EQUAL(SYS_MSG_TEST_PROCESS, Msgs[0]);
}

TEST(Message, SendMessageCycle)
{
  LONGS_EQUAL(0, Msgs.size());
  DMsg_SendCycle(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL, MSG_MSEC(10));
  RunMsgTimer(MSG_MSEC(10));
  RunMsgTimer(MSG_MSEC(10));
  LONGS_EQUAL(2, Msgs.size());
  LONGS_EQUAL(SYS_MSG_TEST_PROCESS, Msgs[0]);
  LONGS_EQUAL(SYS_MSG_TEST_PROCESS, Msgs[1]);
}

TEST(Message, SendTwoMessages)
{
  LONGS_EQUAL(0, Msgs.size());
  DMsg_SendCycle(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL, MSG_MSEC(8));
  DMsg_SendCycle(&MessageProgress, SYS_MSG_CONTINUE_PROCESS, NULL, MSG_MSEC(9));
  RunMsgTimer(MSG_MSEC(8));
  RunMsgTimer(MSG_MSEC(1));
  LONGS_EQUAL(2, Msgs.size());
  LONGS_EQUAL(SYS_MSG_TEST_PROCESS, Msgs[0]);
  LONGS_EQUAL(SYS_MSG_CONTINUE_PROCESS, Msgs[1]);
}

TEST(Message, SendThreeMessages)
{
  LONGS_EQUAL(0, Msgs.size());
  DMsg_SendCycle(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL, MSG_MSEC(8));
  DMsg_SendCycle(&MessageProgress, SYS_MSG_POLL_PROCESS, NULL, MSG_MSEC(7));
  DMsg_SendCycle(&MessageProgress, SYS_MSG_CONTINUE_PROCESS, NULL, MSG_MSEC(9));
  RunMsgTimer(MSG_MSEC(10));
  LONGS_EQUAL(3, Msgs.size());
  LONGS_EQUAL(SYS_MSG_POLL_PROCESS, Msgs[0]);
  LONGS_EQUAL(SYS_MSG_TEST_PROCESS, Msgs[1]);
  LONGS_EQUAL(SYS_MSG_CONTINUE_PROCESS, Msgs[2]);
}

TEST(Message, CancelFirstMessage)
{
  LONGS_EQUAL(0, Msgs.size());
  DMsg_SendLater(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL, MSG_MSEC(10));
  DMsg_SendLater(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL, MSG_MSEC(1));

  DMsg_CancelFirst(&MessageProgress, SYS_MSG_TEST_PROCESS);
  RunMsgTimer(MSG_MSEC(10));

  LONGS_EQUAL(1, Msgs.size());
  LONGS_EQUAL(SYS_MSG_TEST_PROCESS, Msgs[0]);
}

TEST(Message, CancelAllMessages)
{
  LONGS_EQUAL(0, Msgs.size());
  DMsg_SendLater(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL, MSG_MSEC(10));
  DMsg_SendLater(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL, MSG_MSEC(1));

  DMsg_Cancel(&MessageProgress, SYS_MSG_TEST_PROCESS);
  RunMsgTimer(MSG_MSEC(10));
  RunMsgTimer(MSG_MSEC(10));
  LONGS_EQUAL(0, Msgs.size());
}

TEST(Message, CancelCycleMessages)
{
  LONGS_EQUAL(0, Msgs.size());
  DMsg_SendCycle(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL, MSG_MSEC(10));
  DMsg_Cancel(&MessageProgress, SYS_MSG_TEST_PROCESS);
  RunMsgTimer(MSG_MSEC(10));
  LONGS_EQUAL(0, Msgs.size());
}

TEST(Message, FlushMessages)
{
  LONGS_EQUAL(0, Msgs.size());
  DMsg_SendLater(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL, MSG_MSEC(10));
  DMsg_SendLater(&MessageProgress, SYS_MSG_POLL_PROCESS, NULL, MSG_MSEC(1));

  DMsg_Flush(&MessageProgress);
  RunMsgTimer(MSG_MSEC(10));
  RunMsgTimer(MSG_MSEC(10));

  LONGS_EQUAL(0, Msgs.size());
}

TEST(Message, CancelAllMessagesInTaskList)
{
  LONGS_EQUAL(0, Msgs.size());
  DMsg_SendInstant(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL);
  DMsg_SendInstant(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL);

  DMsg_Cancel(&MessageProgress, SYS_MSG_TEST_PROCESS);
  RunMsgTimer(MSG_MSEC(10));

  LONGS_EQUAL(0, Msgs.size());
}

TEST(Message, FlushMessagesInTaskList)
{
  LONGS_EQUAL(0, Msgs.size());
  DMsg_SendInstant(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL);
  DMsg_SendInstant(&MessageProgress, SYS_MSG_POLL_PROCESS, NULL);

  DMsg_Flush(&MessageProgress);
  RunMsgTimer(MSG_MSEC(10));

  LONGS_EQUAL(0, Msgs.size());
}

TEST(Message, FreeArgInCancellingMessages)
{
  void *arg0 = DMem_Malloc(5);
  DMsg_SendLater(&MessageProgress, SYS_MSG_TEST_PROCESS, arg0, MSG_MSEC(10));
  DMsg_Cancel(&MessageProgress, SYS_MSG_TEST_PROCESS);

  // memory of arg0 is released, it is reused by arg1
  void *arg1 = DMem_Malloc(5);
  POINTERS_EQUAL(arg1, arg0);
}

TEST(Message, FreeArgInHandlingMessages)
{
  void *arg0 = DMem_Malloc(5);
  DMsg_SendLater(&MessageProgress, SYS_MSG_TEST_PROCESS, arg0, MSG_MSEC(10));
  RunMsgTimer(MSG_MSEC(10));

  // memory of arg0 is released, it is reused by arg1
  void *arg1 = DMem_Malloc(5);
  POINTERS_EQUAL(arg1, arg0);
}

TEST(Message, RemianTime)
{
  LONGS_EQUAL(0, Msgs.size());
  DMsg_SendLater(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL, MSG_MSEC(10));
  LONGS_EQUAL(10, DMsg_GetRemainTime(&MessageProgress, SYS_MSG_TEST_PROCESS));

  RunMsgTimer(MSG_MSEC(1));
  LONGS_EQUAL(9, DMsg_GetRemainTime(&MessageProgress, SYS_MSG_TEST_PROCESS));

  RunMsgTimer(MSG_MSEC(9));
  LONGS_EQUAL(0, DMsg_GetRemainTime(&MessageProgress, SYS_MSG_TEST_PROCESS));
}

TEST(Message, IsMessageInProcess)
{
  DMsg_SendLater(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL, MSG_MSEC(10));
  CHECK_TRUE(DMsg_IsMsgInProcess(&MessageProgress, SYS_MSG_TEST_PROCESS));

  RunMsgTimer(MSG_MSEC(10));
  CHECK_FALSE(DMsg_IsMsgInProcess(&MessageProgress, SYS_MSG_TEST_PROCESS));
}

TEST(Message, DoNotHandleMessageWhenProcessIsIdle)
{
  Process_Exit(&MessageProgress);
  DMsg_SendInstant(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL);
  CHECK_FALSE(DTask_IsMsgInTask(&MessageProgress, SYS_MSG_TEST_PROCESS));

  DMsg_SendLater(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL, MSG_MSEC(10));
  CHECK_FALSE(DMsg_IsMsgInProcess(&MessageProgress, SYS_MSG_TEST_PROCESS));
}