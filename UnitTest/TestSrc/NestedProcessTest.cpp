#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport_c.h"

#include "CleanVector.h"
#include "SendMsgToTask.h"
#include "TestReset.h"
#include "message.h"
#include "process.h"
#include <vector>

using std::vector;

PROCESS(MainProgress);
PROCESS(SubProgress);

static vector<MsgId_t> SubMsgs{};
PROCESS_HANDLER(SubProgress, msgId, arg)
{
  PROCESS_SCHEDULE_BEGIN();

  SubMsgs.push_back(msgId);
  PROCESS_WAIT_FOR_MSG(msgId == SYS_MSG_EXIT_PROCESS);

  SendMsgToTask(&MainProgress, SYS_MSG_PROGRESS_EXITED, CreateProcessArg(&SubProgress));

  PROCESS_SCHEDULE_END();
}

PROCESS_HANDLER(MainProgress, msgId, arg)
{
  PROCESS_SCHEDULE_BEGIN();
  Process_Start(&SubProgress);

  PROCESS_WAIT_FOR_MSG(msgId == SYS_MSG_PROGRESS_EXITED && arg != NULL &&
                       ((ProcessArg_t *)arg)->Process == &SubProgress);

  PROCESS_SCHEDULE_END();
}

TEST_GROUP(ProcessNested){TEST_SETUP(){TestOutput::enableCompactFormat();
}

TEST_TEARDOWN()
{
  CleanVector(SubMsgs);
  TestReset();
  mock_c()->checkExpectations();
  mock_c()->clear();
  TestOutput::disableCompactFormat();
}
}
;

TEST(ProcessNested, StartSubProcessFromMainProgress)
{
  Process_Start(&MainProgress);
  LONGS_EQUAL(1, SubMsgs.size());
  LONGS_EQUAL(SYS_MSG_START_PROGRESS, SubMsgs[0]);
}

TEST(ProcessNested, WaitingForEndOfSubProcess)
{
  Process_Start(&MainProgress);
  LONGS_EQUAL(true, Process_IsRunning(&MainProgress));
  SendMsgToTask(&SubProgress, SYS_MSG_EXIT_PROCESS, NULL);
  LONGS_EQUAL(false, Process_IsRunning(&MainProgress));
}