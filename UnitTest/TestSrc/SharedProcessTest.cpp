#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport_c.h"

#include "CleanVector.h"
#include "SendMsgToTask.h"
#include "TestReset.h"
#include "d_message.h"
#include "d_process.h"
#include "d_task.h"
#include <vector>

using std::vector;

static DProcess_t Process0;
static DProcess_t Process1;

static vector<DMsgId_t>     Msgs{};
static vector<DProcess_t *> Processes{};
static u8                   ProgressHandler(DProcess_t *process, DMsgId_t msgId, DMsgArg_t arg)
{
  Msgs.push_back(msgId);
  Processes.push_back(process);

  PROCESS_SCHEDULE_BEGIN();

  PROCESS_WAIT_FOR_MSG(msgId == SYS_MSG_EXIT_PROCESS);

  PROCESS_SCHEDULE_END();
}

void InitAndStartSharedProcesses(void)
{
  DProcess_InitStruct(&Process0, ProgressHandler, "Process0");
  DProcess_InitStruct(&Process1, ProgressHandler, "Process1");

  DProcess_Start(&Process0);
  DProcess_Start(&Process1);
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
  LONGS_EQUAL(true, DProcess_IsRunning(&Process0));
  LONGS_EQUAL(true, DProcess_IsRunning(&Process1));

  SendMsgToTask(&Process0, SYS_MSG_EXIT_PROCESS, NULL);
  LONGS_EQUAL(false, DProcess_IsRunning(&Process0));
  LONGS_EQUAL(true, DProcess_IsRunning(&Process1));

  SendMsgToTask(&Process1, SYS_MSG_EXIT_PROCESS, NULL);
  LONGS_EQUAL(false, DProcess_IsRunning(&Process0));
  LONGS_EQUAL(false, DProcess_IsRunning(&Process1));
}

TEST(ProcessShared, MultipleInitializtion)
{
  DProcess_InitStruct(&Process0, ProgressHandler, "Process0");
  DProcess_InitStruct(&Process1, ProgressHandler, "Process1");

  LONGS_EQUAL(2, Msgs.size());
  LONGS_EQUAL(SYS_MSG_START_PROGRESS, Msgs[0]);
  LONGS_EQUAL(SYS_MSG_START_PROGRESS, Msgs[1]);

  LONGS_EQUAL(2, Processes.size());
  POINTERS_EQUAL(&Process0, Processes[0]);
  POINTERS_EQUAL(&Process1, Processes[1]);
}