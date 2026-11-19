#include <gtest/gtest.h>

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

class ProcessShared : public ::testing::Test
{
protected:
  void SetUp() override
  {
    InitAndStartSharedProcesses();
  }

  void TearDown() override
  {
    CleanVector(Msgs);
    CleanVector(Processes);

    TestReset();
  }
};

TEST_F(ProcessShared, SharedHandler)
{
  EXPECT_EQ(2, Msgs.size());
  EXPECT_EQ(SYS_MSG_START_PROGRESS, Msgs[0]);
  EXPECT_EQ(SYS_MSG_START_PROGRESS, Msgs[1]);

  EXPECT_EQ(2, Processes.size());
  EXPECT_EQ(&Process0, Processes[0]);
  EXPECT_EQ(&Process1, Processes[1]);
}

TEST_F(ProcessShared, SharedScheduleSection)
{
  EXPECT_TRUE(DProcess_IsRunning(&Process0));
  EXPECT_TRUE(DProcess_IsRunning(&Process1));

  SendMsgToTask(&Process0, SYS_MSG_EXIT_PROCESS, NULL);
  EXPECT_FALSE(DProcess_IsRunning(&Process0));
  EXPECT_TRUE(DProcess_IsRunning(&Process1));

  SendMsgToTask(&Process1, SYS_MSG_EXIT_PROCESS, NULL);
  EXPECT_FALSE(DProcess_IsRunning(&Process0));
  EXPECT_FALSE(DProcess_IsRunning(&Process1));
}

TEST_F(ProcessShared, MultipleInitializtion)
{
  DProcess_InitStruct(&Process0, ProgressHandler, "Process0");
  DProcess_InitStruct(&Process1, ProgressHandler, "Process1");

  EXPECT_EQ(2, Msgs.size());
  EXPECT_EQ(SYS_MSG_START_PROGRESS, Msgs[0]);
  EXPECT_EQ(SYS_MSG_START_PROGRESS, Msgs[1]);

  EXPECT_EQ(2, Processes.size());
  EXPECT_EQ(&Process0, Processes[0]);
  EXPECT_EQ(&Process1, Processes[1]);
}
