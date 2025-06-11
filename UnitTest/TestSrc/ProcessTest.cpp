#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport_c.h"

#include "SendMsgToTask.h"
#include "TestReset.h"
#include "message.h"
#include "process.h"
#include "task.h"
#include <vector>

using std::vector;

PROCESS(AnyMsgProgress);
PROCESS(SpecifiedMsgProgress);
PROCESS(PollMsgProgress);
PROCESS(ResetProgress);

static vector<MsgId_t> AnyMsgs{};
static bool AnyMsgReceived = false;
PROCESS_HANDLER(AnyMsgProgress, msgId, arg)
{
  AnyMsgs.push_back(msgId);

  PROCESS_SCHEDULE_BEGIN();

  AnyMsgReceived = false;
  PROCESS_WAIT_FOR_ANY_MSG();
  AnyMsgReceived = true;

  PROCESS_SCHEDULE_END();
}

static bool SpecifiedMsgReceived = false;
static vector<MsgId_t> SpecifiedMsgs{};
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
  Process_Exit(&ResetProgress);

  PROCESS_SCHEDULE_END();
}

TEST_GROUP(Process){TEST_SETUP(){Process_Start(&AnyMsgProgress);
Process_Start(&SpecifiedMsgProgress);
Process_Start(&PollMsgProgress);
Process_Start(&ResetProgress);

TestOutput::enableCompactFormat();
}

TEST_TEARDOWN()
{
  AnyMsgs.clear();
  AnyMsgs.shrink_to_fit();

  SpecifiedMsgs.clear();         // destroy the object held in vector
  SpecifiedMsgs.shrink_to_fit(); // deallocate the memory of vector

  TestReset();
  mock_c()->checkExpectations();
  mock_c()->clear();
  TestOutput::disableCompactFormat();
}
}
;

TEST(Process, ProcessHandleMsg)
{
  LONGS_EQUAL(false, SpecifiedMsgReceived);
  Process_HandleMsg(&SpecifiedMsgProgress, SYS_MSG_CONTINUE_PROCESS, NULL);
  LONGS_EQUAL(true, SpecifiedMsgReceived);
}

TEST(Process, SpecifiedMsgReceived)
{
  LONGS_EQUAL(false, SpecifiedMsgReceived);
  SendMsgToTask(&SpecifiedMsgProgress, SYS_MSG_CONTINUE_PROCESS, NULL);
  LONGS_EQUAL(true, SpecifiedMsgReceived);
}

TEST(Process, AnyMsgReceived)
{
  LONGS_EQUAL(false, AnyMsgReceived);
  SendMsgToTask(&AnyMsgProgress, SYS_MSG_CONTINUE_PROCESS, NULL);
  LONGS_EQUAL(true, AnyMsgReceived);
}

TEST(Process, BroadcastMsgReceived)
{
  LONGS_EQUAL(false, AnyMsgReceived);
  LONGS_EQUAL(false, SpecifiedMsgReceived);
  SendMsgToTask(&BroadcastProcess, SYS_MSG_CONTINUE_PROCESS, NULL);
  LONGS_EQUAL(true, SpecifiedMsgReceived);
  LONGS_EQUAL(true, AnyMsgReceived);
}

TEST(Process, BroadcastMsgReceivedLater)
{
  LONGS_EQUAL(false, AnyMsgReceived);
  LONGS_EQUAL(false, SpecifiedMsgReceived);
  Msg_SendLater(&BroadcastProcess, SYS_MSG_CONTINUE_PROCESS, NULL, MSG_MSEC(10));
  RunMsgTimer(MSG_MSEC(10));
  LONGS_EQUAL(true, SpecifiedMsgReceived);
  LONGS_EQUAL(true, AnyMsgReceived);
}

TEST(Process, ExitProgress)
{
  // progress already started in setup
  Process_Exit(&SpecifiedMsgProgress);
  LONGS_EQUAL(false, Process_IsRunning(&SpecifiedMsgProgress));

  LONGS_EQUAL(false, SpecifiedMsgReceived);
  SendMsgToTask(&SpecifiedMsgProgress, SYS_MSG_CONTINUE_PROCESS, NULL);
  LONGS_EQUAL(false, SpecifiedMsgReceived);
}

TEST(Process, PollIdleProgress)
{
  Process_Exit(&PollMsgProgress);
  Process_Poll(&PollMsgProgress);
  LONGS_EQUAL(false, PollMsgProgress.NeedPoll);
}

TEST(Process, PollProgress)
{
  LONGS_EQUAL(false, PollMsgReceived);
  Process_Poll(&PollMsgProgress);
  Process_Run();
  LONGS_EQUAL(true, PollMsgReceived);
}

TEST(Process, StartProgress)
{
  // progress already started in setup
  LONGS_EQUAL(SYS_MSG_START_PROGRESS, AnyMsgs[0]);
  LONGS_EQUAL(SYS_MSG_START_PROGRESS, SpecifiedMsgs[0]);
}

TEST(Process, StartProgressMultipleTimes)
{
  Process_Start(&AnyMsgProgress);
  Process_Start(&AnyMsgProgress);
  LONGS_EQUAL(1, AnyMsgs.size());
  LONGS_EQUAL(SYS_MSG_START_PROGRESS, AnyMsgs[0]);
}

TEST(Process, ReStartProgress)
{
  // progress already started in setup
  Process_ReStart(&AnyMsgProgress);
  LONGS_EQUAL(2, AnyMsgs.size());
  LONGS_EQUAL(SYS_MSG_START_PROGRESS, AnyMsgs[0]);
  LONGS_EQUAL(SYS_MSG_START_PROGRESS, AnyMsgs[1]);
}

TEST(Process, ClearAllPendingMsg_WhenExitProgress)
{
  Msg_SendLater(&AnyMsgProgress, SYS_MSG_CONTINUE_PROCESS, NULL, MSG_MSEC(10));
  Process_Exit(&AnyMsgProgress);
  AnyMsgs.clear();

  // do not receive SYS_MSG_CONTINUE_PROCESS when restart the progress
  Process_Start(&AnyMsgProgress);
  RunMsgTimer(MSG_MSEC(10));
  LONGS_EQUAL(1, AnyMsgs.size());
  LONGS_EQUAL(SYS_MSG_START_PROGRESS, AnyMsgs[0]);
}

TEST(Process, ExitAllProcessWhenInit)
{
  Process_Init();
  LONGS_EQUAL(false, Process_IsRunning(&AnyMsgProgress));
  LONGS_EQUAL(false, Process_IsRunning(&SpecifiedMsgProgress));
  LONGS_EQUAL(false, Process_IsRunning(&PollMsgProgress));
}

TEST(Process, ExitProcessInHandler)
{
  SendMsgToTask(&ResetProgress, SYS_MSG_CONTINUE_PROCESS, NULL);
  LONGS_EQUAL(false, Process_IsRunning(&ResetProgress));

  Msg_SendLater(&ResetProgress, SYS_MSG_CONTINUE_PROCESS, NULL, MSG_MSEC(10));
  CHECK_FALSE(Msg_IsMsgInProcess(&ResetProgress, SYS_MSG_CONTINUE_PROCESS));
}

TEST(Process, StartUnInitiliazedProcess)
{
  static Process_t Test;
  Process_Start(&Test);
  LONGS_EQUAL(false, Process_IsRunning(&Test));
}
