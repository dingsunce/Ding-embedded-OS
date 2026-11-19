#include <gtest/gtest.h>

#include "TestReset.h"
#include "d_tick.h"

static bool CallBack200usCalled = false;
static bool CallBack1msCalled = false;

class SysTickTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    CallBack200usCalled = false;
    CallBack1msCalled = false;
  }

  void TearDown() override
  {
    TestReset();
  }
};

TEST_F(SysTickTest, GenerateTick)
{
  DTick_On();
  EXPECT_EQ(1, DTick_GetCurTicks());
  DTick_Reset();
  EXPECT_EQ(0, DTick_GetCurTicks());

  DTick_On();
  EXPECT_EQ(1, DTick_Now());
  DTick_Reset();
  EXPECT_EQ(0, DTick_Now());
}

TEST_F(SysTickTest, DTick_Update)
{
  DTick_Update(10);
  EXPECT_EQ(10, DTick_Now());

  DTick_Update(0xffffffff); // 0xffffffff + 1 is 0
  EXPECT_EQ(9, DTick_Now());
}
