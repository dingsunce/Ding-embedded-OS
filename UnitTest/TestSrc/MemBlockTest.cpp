#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport_c.h"

#include "TestReset.h"
#include "memb.h"

typedef struct MembTest
{
  u8 Id;
} MembTest_t;

MEMB(MembTestMem, MembTest_t, 10);

TEST_GROUP(MemoryBlock){TEST_SETUP(){Memb_Init(&MembTestMem);
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

TEST(MemoryBlock, AllocateAndFree)
{
  void* ptr0 = Memb_Alloc(&MembTestMem);
  void* ptr1 = Memb_Alloc(&MembTestMem);
  LONGS_EQUAL(true, Memb_InMem(&MembTestMem, ptr0))
  LONGS_EQUAL(true, Memb_InMem(&MembTestMem, ptr1))
  LONGS_EQUAL(8, Memb_NumFree(&MembTestMem))

  Memb_Free(&MembTestMem, ptr0);
  Memb_Free(&MembTestMem, ptr1);
  LONGS_EQUAL(false, Memb_InMem(&MembTestMem, ptr0))
  LONGS_EQUAL(false, Memb_InMem(&MembTestMem, ptr1))
  LONGS_EQUAL(10, Memb_NumFree(&MembTestMem))
}