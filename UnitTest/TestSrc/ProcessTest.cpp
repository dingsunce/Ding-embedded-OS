#include <gtest/gtest.h>

#include "SendMsgToTask.h"
#include "TestReset.h"
#include "d_message.h"
#include "d_process.h"
#include "d_task.h"
#include <vector>

using std::vector;

PROCESS(AnyMsgProgress);
PROCESS(SpecifiedMsgProgress);
PROCESS(PollMsgProgress);
PROCESS(ResetProgress);

static vector<DMsgId_t> AnyMsgs{};
static bool             AnyMsgReceived = false;
PROCESS_HANDLER(AnyMsgProgress, msgId, arg)
{
  AnyMsgs.push_back(msgId);

  PROCESS_SCHEDULE_BEGIN();

  AnyMsgReceived = false;
  /*
    PROCESS_WAIT_FOR_ANY_MSG will not handle SYS_MSG_START_PROGRESS
    it will discard it
  */
  PROCESS_WAIT_FOR_ANY_MSG();
  AnyMsgReceived = true;

  PROCESS_SCHEDULE_END();
}

static bool             SpecifiedMsgReceived = false;
static vector<DMsgId_t> SpecifiedMsgs{};
PROCESS_HANDLER(SpecifiedMsgProgress, msgId, arg)
{
  SpecifiedMsgs.push_back(msgId);

  PROCESS_SCHEDULE_BEGIN();

  SpecifiedMsgReceived = false;
  PROCESS_WAIT_FOR_MSG(msgId == SYS_MSG_CONTINUE_PROCESS);
  SpecifiedMsgReceived = true;

  PROCESS_SCHEDULE_END();
}

static bool PollMsgReceived = false;
PROCESS_HANDLER(PollMsgProgress, msgId, arg)
{
  PROCESS_SCHEDULE_BEGIN();

  PollMsgReceived = false;
  PROCESS_WAIT_FOR_MSG(msgId == SYS_MSG_POLL_PROCESS);
  PollMsgReceived = true;

  PROCESS_SCHEDULE_END();
}

PROCESS_HANDLER(ResetProgress, msgId, arg)
{
  PROCESS_SCHEDULE_BEGIN();

  PROCESS_WAIT_FOR_MSG(msgId == SYS_MSG_CONTINUE_PROCESS);
  DProcess_ExitProc(&ResetProgress);

  PROCESS_SCHEDULE_END();
}

class Process : public ::testing::Test
{
protected:
  void SetUp() override
  {
    DProcess_Start(&AnyMsgProgress);
    DProcess_Start(&SpecifiedMsgProgress);
    DProcess_Start(&PollMsgProgress);
    DProcess_Start(&ResetProgress);
  }

  void TearDown() override
  {
    AnyMsgs.clear();
    AnyMsgs.shrink_to_fit();

    SpecifiedMsgs.clear();         // destroy the object held in vector
    SpecifiedMsgs.shrink_to_fit(); // deallocate the memory of vector

    TestReset();
  }
};

TEST_F(Process, ProcessHandleMsg)
{
  EXPECT_FALSE(SpecifiedMsgReceived);
  DProcess_HandleMsg(&SpecifiedMsgProgress, SYS_MSG_CONTINUE_PROCESS, NULL);
  EXPECT_TRUE(SpecifiedMsgReceived);
}

TEST_F(Process, SpecifiedMsgReceived)
{
  EXPECT_FALSE(SpecifiedMsgReceived);
  SendMsgToTask(&SpecifiedMsgProgress, SYS_MSG_CONTINUE_PROCESS, NULL);
  EXPECT_TRUE(SpecifiedMsgReceived);
}

TEST_F(Process, AnyMsgReceived)
{
  EXPECT_FALSE(AnyMsgReceived);
  SendMsgToTask(&AnyMsgProgress, SYS_MSG_CONTINUE_PROCESS, NULL);
  EXPECT_TRUE(AnyMsgReceived);
}

TEST_F(Process, ExitProgress)
{
  // process already started in setup
  DProcess_ExitProc(&SpecifiedMsgProgress);
  EXPECT_FALSE(DProcess_IsRunning(&SpecifiedMsgProgress));

  EXPECT_FALSE(SpecifiedMsgReceived);
  SendMsgToTask(&SpecifiedMsgProgress, SYS_MSG_CONTINUE_PROCESS, NULL);
  EXPECT_FALSE(SpecifiedMsgReceived);
}

TEST_F(Process, PollIdleProgress)
{
  DProcess_ExitProc(&PollMsgProgress);
  DProcess_Poll(&PollMsgProgress);
  EXPECT_FALSE(PollMsgProgress.NeedPoll);
}

TEST_F(Process, PollProgress)
{
  EXPECT_FALSE(PollMsgReceived);
  DProcess_Poll(&PollMsgProgress);
  DProcess_Run();
  DTask_Run();
  EXPECT_TRUE(PollMsgReceived);
}

TEST_F(Process, StartProgress)
{
  // process already started in setup
  EXPECT_EQ(SYS_MSG_START_PROGRESS, AnyMsgs[0]);
  EXPECT_EQ(SYS_MSG_START_PROGRESS, SpecifiedMsgs[0]);
}

TEST_F(Process, StartProgressMultipleTimes)
{
  DProcess_Start(&AnyMsgProgress);
  DProcess_Start(&AnyMsgProgress);
  EXPECT_EQ(1, AnyMsgs.size());
  EXPECT_EQ(SYS_MSG_START_PROGRESS, AnyMsgs[0]);
}

TEST_F(Process, ReStartProgress)
{
  // process already started in setup
  DProcess_ReStart(&AnyMsgProgress);
  EXPECT_EQ(2, AnyMsgs.size());
  EXPECT_EQ(SYS_MSG_START_PROGRESS, AnyMsgs[0]);
  EXPECT_EQ(SYS_MSG_START_PROGRESS, AnyMsgs[1]);
}

TEST_F(Process, ClearAllPendingMsg_WhenExitProgress)
{
  DMsg_SendLater(&AnyMsgProgress, SYS_MSG_CONTINUE_PROCESS, NULL, MSG_MSEC(10));
  DProcess_ExitProc(&AnyMsgProgress);
  AnyMsgs.clear();

  // do not receive SYS_MSG_CONTINUE_PROCESS when restart the progress
  DProcess_Start(&AnyMsgProgress);
  RunMsgTimer(MSG_MSEC(10));
  EXPECT_EQ(1, AnyMsgs.size());
  EXPECT_EQ(SYS_MSG_START_PROGRESS, AnyMsgs[0]);
}

TEST_F(Process, ExitAllProcessWhenInit)
{
  DProcess_Init();
  EXPECT_FALSE(DProcess_IsRunning(&AnyMsgProgress));
  EXPECT_FALSE(DProcess_IsRunning(&SpecifiedMsgProgress));
  EXPECT_FALSE(DProcess_IsRunning(&PollMsgProgress));
}

TEST_F(Process, ExitProcessInHandler)
{
  SendMsgToTask(&ResetProgress, SYS_MSG_CONTINUE_PROCESS, NULL);
  EXPECT_FALSE(DProcess_IsRunning(&ResetProgress));

  DMsg_SendLater(&ResetProgress, SYS_MSG_CONTINUE_PROCESS, NULL, MSG_MSEC(10));
  EXPECT_FALSE(DMsg_IsMsgInProcess(&ResetProgress, SYS_MSG_CONTINUE_PROCESS));
}

TEST_F(Process, StartUnInitiliazedProcess)
{
  static DProcess_t Test;
  DProcess_Start(&Test);
  EXPECT_FALSE(DProcess_IsRunning(&Test));
}
