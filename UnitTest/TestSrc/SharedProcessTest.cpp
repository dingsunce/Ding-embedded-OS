#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport_c.h"

#include "CleanVector.h"
#include "SendMsgToTask.h"
#include "TestReset.h"
#include "d_task.h"
#include "message.h"
#include "process.h"
#include <vector>

using std::vector;

static Process_t Process0;
static Process_t Process1;

static vector<MsgId_t>     Msgs{};
static vector<Process_t *> Processes{};
static u8                  ProgressHandler(Process_t *process, MsgId_t msgId, MsgArg_t arg)
{
  Msgs.push_back(msgId);
  Processes.push_back(process);

  PROCESS_SCHEDULE_BEGIN();

  PROCESS_WAIT_FOR_MSG(msgId == SYS_MSG_EXIT_PROCESS);

  PROCESS_SCHEDULE_END();
}

void InitAndStartSharedProcesses(void)
{
  Process_InitStruct(&Process0, ProgressHandler, "Process0");
  Process_InitStruct(&Process1, ProgressHandler, "Process1");

  Process_Start(&Process0);
  Process_Start(&Process1);
}

TEST_GROUP(ProcessShared){TEST_SETUP(){InitAndStartSharedProcesses();
TestOutput::enableCompactFormat();
}

TEST_TEARDOWN()
{
  CleanVector(Msgs);
  CleanVector(Processes);

  TestReset();
  mock_c()->checkExpectations();
  mock_c()->clear();
  TestOutput::disableCompactFormat();
}
}
;

TEST(ProcessShared, SharedHandler)
{
  LONGS_EQUAL(2, Msgs.size());
  LONGS_EQUAL(SYS_MSG_START_PROGRESS, Msgs[0]);
  LONGS_EQUAL(SYS_MSG_START_PROGRESS, Msgs[1]);

  LONGS_EQUAL(2, Processes.size());
  POINTERS_EQUAL(&Process0, Processes[0]);
  POINTERS_EQUAL(&Process1, Processes[1]);
}

TEST(ProcessShared, SharedScheduleSection)
{
  LONGS_EQUAL(true, Process_IsRunning(&Process0));
  LONGS_EQUAL(true, Process_IsRunning(&Process1));

  SendMsgToTask(&Process0, SYS_MSG_EXIT_PROCESS, NULL);
  LONGS_EQUAL(false, Process_IsRunning(&Process0));
  LONGS_EQUAL(true, Process_IsRunning(&Process1));

  SendMsgToTask(&Process1, SYS_MSG_EXIT_PROCESS, NULL);
  LONGS_EQUAL(false, Process_IsRunning(&Process0));
  LONGS_EQUAL(false, Process_IsRunning(&Process1));
}

TEST(ProcessShared, MultipleInitializtion)
{
  Process_InitStruct(&Process0, ProgressHandler, "Process0");
  Process_InitStruct(&Process1, ProgressHandler, "Process1");

  LONGS_EQUAL(2, Msgs.size());
  LONGS_EQUAL(SYS_MSG_START_PROGRESS, Msgs[0]);
  LONGS_EQUAL(SYS_MSG_START_PROGRESS, Msgs[1]);

  LONGS_EQUAL(2, Processes.size());
  POINTERS_EQUAL(&Process0, Processes[0]);
  POINTERS_EQUAL(&Process1, Processes[1]);
}