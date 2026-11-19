#include <gtest/gtest.h>

#include "CleanVector.h"
#include "SendMsgToTask.h"
#include "TestReset.h"
#include "d_message.h"
#include "d_process.h"
#include <vector>

using std::vector;

PROCESS(MainProgress);
PROCESS(SubProgress);

static vector<DMsgId_t> SubMsgs{};
PROCESS_HANDLER(SubProgress, msgId, arg)
{
  PROCESS_SCHEDULE_BEGIN();

  SubMsgs.push_back(msgId);
  PROCESS_WAIT_FOR_MSG(msgId == SYS_MSG_EXIT_PROCESS);

  SendMsgToTask(&MainProgress, SYS_MSG_PROGRESS_EXITED, DProcess_CreateArg(&SubProgress));

  PROCESS_SCHEDULE_END();
}

PROCESS_HANDLER(MainProgress, msgId, arg)
{
  PROCESS_SCHEDULE_BEGIN();
  DProcess_Start(&SubProgress);

  PROCESS_WAIT_FOR_MSG(msgId == SYS_MSG_PROGRESS_EXITED && arg != NULL &&
                       ((DProcessArg_t *)arg)->Process == &SubProgress);

  PROCESS_SCHEDULE_END();
}

class ProcessNested : public ::testing::Test
{
protected:
  void TearDown() override
  {
    CleanVector(SubMsgs);
    TestReset();
  }
};

TEST_F(ProcessNested, StartSubProcessFromMainProgress)
{
  DProcess_Start(&MainProgress);
  EXPECT_EQ(1, SubMsgs.size());
  EXPECT_EQ(SYS_MSG_START_PROGRESS, SubMsgs[0]);
}

TEST_F(ProcessNested, WaitingForEndOfSubProcess)
{
  DProcess_Start(&MainProgress);
  EXPECT_TRUE(DProcess_IsRunning(&MainProgress));
  SendMsgToTask(&SubProgress, SYS_MSG_EXIT_PROCESS, NULL);
  EXPECT_FALSE(DProcess_IsRunning(&MainProgress));
}
