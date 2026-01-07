#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport_c.h"

#include "TestReset.h"
#include "d_tick.h"

static bool CallBack200usCalled = false;
static bool CallBack1msCalled = false;
TEST_GROUP(SysTickTest){TEST_SETUP(){CallBack200usCalled = false;
CallBack1msCalled = false;
TestOutput::enableCompactFormat();
}

TEST_TEARDOWN()
{
  TestReset();
  mock_c()->checkExpectations();
  mock_c()->clear();
  TestOutput::disableCompactFormat();
}
}
;

TEST(SysTickTest, GenerateTick)
{
  DTick_On();
  LONGS_EQUAL(1, DTick_GetCurTicks());
  DTick_Reset();
  LONGS_EQUAL(0, DTick_GetCurTicks());

  DTick_On();
  LONGS_EQUAL(1, DTick_Now());
  DTick_Reset();
  LONGS_EQUAL(0, DTick_Now());
}

TEST(SysTickTest, DTick_Update)
{
  DTick_Update(10);
  LONGS_EQUAL(10, DTick_Now());

  DTick_Update(0xffffffff); // 0xffffffff + 1 is 0
  LONGS_EQUAL(9, DTick_Now());
}
