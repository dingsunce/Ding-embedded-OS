#include <gtest/gtest.h>

#include "CleanVector.h"
#include "SendMsgToTask.h"
#include "TestReset.h"
#include "d_mem.h"
#include "d_message.h"
#include "d_task.h"
#include <vector>

using std::vector;

PROCESS(MessageProgress);

static vector<DMsgId_t> Msgs{};
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

class Message : public ::testing::Test
{
protected:
  void SetUp() override
  {
    DProcess_Start(&MessageProgress);
  }

  void TearDown() override
  {
    CleanVector(Msgs);
    TestReset();
  }
};

TEST_F(Message, SendInstantMessage)
{
  EXPECT_EQ(0, Msgs.size());
  DMsg_SendInstant(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL);
  DTask_Run();
  EXPECT_EQ(1, Msgs.size());
  EXPECT_EQ(SYS_MSG_TEST_PROCESS, Msgs[0]);
}

TEST_F(Message, SendMessageLater)
{
  EXPECT_EQ(0, Msgs.size());
  DMsg_SendLater(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL, MSG_MSEC(1000));
  RunMsgTimer(MSG_MSEC(1000));
  EXPECT_EQ(1, Msgs.size());
  EXPECT_EQ(SYS_MSG_TEST_PROCESS, Msgs[0]);
}

TEST_F(Message, ReSendMessageLater)
{
  EXPECT_EQ(0, Msgs.size());
  DMsg_SendLater(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL, MSG_MSEC(1000));
  DMsg_ReSendLater(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL, MSG_MSEC(1000));
  RunMsgTimer(MSG_MSEC(1000));
  RunMsgTimer(MSG_MSEC(1000));
  EXPECT_EQ(1, Msgs.size());
  EXPECT_EQ(SYS_MSG_TEST_PROCESS, Msgs[0]);
}

TEST_F(Message, SendMessageCycle)
{
  EXPECT_EQ(0, Msgs.size());
  DMsg_SendCycle(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL, MSG_MSEC(10));
  RunMsgTimer(MSG_MSEC(10));
  RunMsgTimer(MSG_MSEC(10));
  EXPECT_EQ(2, Msgs.size());
  EXPECT_EQ(SYS_MSG_TEST_PROCESS, Msgs[0]);
  EXPECT_EQ(SYS_MSG_TEST_PROCESS, Msgs[1]);
}

TEST_F(Message, SendTwoMessages)
{
  EXPECT_EQ(0, Msgs.size());
  DMsg_SendCycle(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL, MSG_MSEC(8));
  DMsg_SendCycle(&MessageProgress, SYS_MSG_CONTINUE_PROCESS, NULL, MSG_MSEC(9));
  RunMsgTimer(MSG_MSEC(8));
  RunMsgTimer(MSG_MSEC(1));
  EXPECT_EQ(2, Msgs.size());
  EXPECT_EQ(SYS_MSG_TEST_PROCESS, Msgs[0]);
  EXPECT_EQ(SYS_MSG_CONTINUE_PROCESS, Msgs[1]);
}

TEST_F(Message, SendThreeMessages)
{
  EXPECT_EQ(0, Msgs.size());
  DMsg_SendCycle(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL, MSG_MSEC(8));
  DMsg_SendCycle(&MessageProgress, SYS_MSG_POLL_PROCESS, NULL, MSG_MSEC(7));
  DMsg_SendCycle(&MessageProgress, SYS_MSG_CONTINUE_PROCESS, NULL, MSG_MSEC(9));
  RunMsgTimer(MSG_MSEC(10));
  EXPECT_EQ(3, Msgs.size());
  EXPECT_EQ(SYS_MSG_POLL_PROCESS, Msgs[0]);
  EXPECT_EQ(SYS_MSG_TEST_PROCESS, Msgs[1]);
  EXPECT_EQ(SYS_MSG_CONTINUE_PROCESS, Msgs[2]);
}

TEST_F(Message, CancelFirstMessage)
{
  EXPECT_EQ(0, Msgs.size());
  DMsg_SendLater(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL, MSG_MSEC(10));
  DMsg_SendLater(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL, MSG_MSEC(1));

  DMsg_CancelFirst(&MessageProgress, SYS_MSG_TEST_PROCESS);
  RunMsgTimer(MSG_MSEC(10));

  EXPECT_EQ(1, Msgs.size());
  EXPECT_EQ(SYS_MSG_TEST_PROCESS, Msgs[0]);
}

TEST_F(Message, CancelAllMessages)
{
  EXPECT_EQ(0, Msgs.size());
  DMsg_SendLater(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL, MSG_MSEC(10));
  DMsg_SendLater(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL, MSG_MSEC(1));

  DMsg_Cancel(&MessageProgress, SYS_MSG_TEST_PROCESS);
  RunMsgTimer(MSG_MSEC(10));
  RunMsgTimer(MSG_MSEC(10));
  EXPECT_EQ(0, Msgs.size());
}

TEST_F(Message, CancelCycleMessages)
{
  EXPECT_EQ(0, Msgs.size());
  DMsg_SendCycle(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL, MSG_MSEC(10));
  DMsg_Cancel(&MessageProgress, SYS_MSG_TEST_PROCESS);
  RunMsgTimer(MSG_MSEC(10));
  EXPECT_EQ(0, Msgs.size());
}

TEST_F(Message, FlushMessages)
{
  EXPECT_EQ(0, Msgs.size());
  DMsg_SendLater(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL, MSG_MSEC(10));
  DMsg_SendLater(&MessageProgress, SYS_MSG_POLL_PROCESS, NULL, MSG_MSEC(1));

  DMsg_Flush(&MessageProgress);
  RunMsgTimer(MSG_MSEC(10));
  RunMsgTimer(MSG_MSEC(10));

  EXPECT_EQ(0, Msgs.size());
}

TEST_F(Message, CancelAllMessagesInTaskList)
{
  EXPECT_EQ(0, Msgs.size());
  DMsg_SendInstant(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL);
  DMsg_SendInstant(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL);

  DMsg_Cancel(&MessageProgress, SYS_MSG_TEST_PROCESS);
  RunMsgTimer(MSG_MSEC(10));

  EXPECT_EQ(0, Msgs.size());
}

TEST_F(Message, FlushMessagesInTaskList)
{
  EXPECT_EQ(0, Msgs.size());
  DMsg_SendInstant(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL);
  DMsg_SendInstant(&MessageProgress, SYS_MSG_POLL_PROCESS, NULL);

  DMsg_Flush(&MessageProgress);
  RunMsgTimer(MSG_MSEC(10));

  EXPECT_EQ(0, Msgs.size());
}

TEST_F(Message, FreeArgInCancellingMessages)
{
  void *arg0 = DMem_Malloc(5);
  DMsg_SendLater(&MessageProgress, SYS_MSG_TEST_PROCESS, arg0, MSG_MSEC(10));
  DMsg_Cancel(&MessageProgress, SYS_MSG_TEST_PROCESS);

  // memory of arg0 is released, it is reused by arg1
  void *arg1 = DMem_Malloc(5);
  EXPECT_EQ(arg1, arg0);
}

TEST_F(Message, FreeArgInHandlingMessages)
{
  void *arg0 = DMem_Malloc(5);
  DMsg_SendLater(&MessageProgress, SYS_MSG_TEST_PROCESS, arg0, MSG_MSEC(10));
  RunMsgTimer(MSG_MSEC(10));

  // memory of arg0 is released, it is reused by arg1
  void *arg1 = DMem_Malloc(5);
  EXPECT_EQ(arg1, arg0);
}

TEST_F(Message, RemianTime)
{
  EXPECT_EQ(0, Msgs.size());
  DMsg_SendLater(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL, MSG_MSEC(10));
  EXPECT_EQ(10, DMsg_GetRemainTime(&MessageProgress, SYS_MSG_TEST_PROCESS));

  RunMsgTimer(MSG_MSEC(1));
  EXPECT_EQ(9, DMsg_GetRemainTime(&MessageProgress, SYS_MSG_TEST_PROCESS));

  RunMsgTimer(MSG_MSEC(9));
  EXPECT_EQ(0, DMsg_GetRemainTime(&MessageProgress, SYS_MSG_TEST_PROCESS));
}

TEST_F(Message, IsMessageInProcess)
{
  DMsg_SendLater(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL, MSG_MSEC(10));
  EXPECT_TRUE(DMsg_IsMsgInProcess(&MessageProgress, SYS_MSG_TEST_PROCESS));

  RunMsgTimer(MSG_MSEC(10));
  EXPECT_FALSE(DMsg_IsMsgInProcess(&MessageProgress, SYS_MSG_TEST_PROCESS));
}

TEST_F(Message, DoNotHandleMessageWhenProcessIsIdle)
{
  DProcess_ExitProc(&MessageProgress);
  DMsg_SendInstant(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL);
  EXPECT_FALSE(DTask_IsMsgInTask(&MessageProgress, SYS_MSG_TEST_PROCESS));

  DMsg_SendLater(&MessageProgress, SYS_MSG_TEST_PROCESS, NULL, MSG_MSEC(10));
  EXPECT_FALSE(DMsg_IsMsgInProcess(&MessageProgress, SYS_MSG_TEST_PROCESS));
}
