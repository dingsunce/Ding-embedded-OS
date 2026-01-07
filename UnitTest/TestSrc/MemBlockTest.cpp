#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport_c.h"

#include "TestReset.h"
#include "d_memb.h"

typedef struct MembTest
{
  u8 Id;
} MembTest_t;

DMEMB(MembTestMem, MembTest_t, 10);

TEST_GROUP(MemoryBlock){TEST_SETUP(){DMemb_Init(&MembTestMem);
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
  void *ptr0 = DMemb_Alloc(&MembTestMem);
  void *ptr1 = DMemb_Alloc(&MembTestMem);
  LONGS_EQUAL(true, DMemb_InMem(&MembTestMem, ptr0))
  LONGS_EQUAL(true, DMemb_InMem(&MembTestMem, ptr1))
  LONGS_EQUAL(8, DMemb_NumFree(&MembTestMem))

  DMemb_Free(&MembTestMem, ptr0);
  DMemb_Free(&MembTestMem, ptr1);
  LONGS_EQUAL(false, DMemb_InMem(&MembTestMem, ptr0))
  LONGS_EQUAL(false, DMemb_InMem(&MembTestMem, ptr1))
  LONGS_EQUAL(10, DMemb_NumFree(&MembTestMem))
}