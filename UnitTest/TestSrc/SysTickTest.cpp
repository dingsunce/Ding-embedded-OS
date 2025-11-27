#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport_c.h"

#include "SysTick.h"
#include "TestReset.h"

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
  SysTick_On();
  LONGS_EQUAL(1, SysTick_Get1msTicks());
  SysTick_Reset();
  LONGS_EQUAL(0, SysTick_Get1msTicks());

  SysTick_On();
  LONGS_EQUAL(1, sys_now());
  SysTick_Reset();
  LONGS_EQUAL(0, sys_now());
}

TEST(SysTickTest, SysTick_Update)
{
  SysTick_Update(10);
  LONGS_EQUAL(10, sys_now());

  SysTick_Update(0xffffffff); // 0xffffffff + 1 is 0
  LONGS_EQUAL(9, sys_now());
}
