#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport_c.h"

#include "TestReset.h"
#include "d_mem.h"

TEST_GROUP(Memory){TEST_SETUP(){TestOutput::enableCompactFormat();
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

TEST(Memory, Allocate)
{
  DMem_Malloc(10);
  LONGS_EQUAL(10 + MEM_HEADER_SIZE, DMem_GetAllocSize());
  LONGS_EQUAL(DMEM_HEAP - 10 - MEM_HEADER_SIZE, DMem_GetFreeSize());

  DMem_Malloc(20);
  LONGS_EQUAL(30 + 2 * MEM_HEADER_SIZE, DMem_GetAllocSize());

  DMem_Malloc(20);
  LONGS_EQUAL(50 + 3 * MEM_HEADER_SIZE, DMem_GetAllocSize());

  DMem_Malloc(20);
  LONGS_EQUAL(70 + 4 * MEM_HEADER_SIZE, DMem_GetAllocSize());

  DMem_Malloc(1024);
  LONGS_EQUAL(70 + 1024 + 5 * MEM_HEADER_SIZE, DMem_GetAllocSize());

  DMem_Malloc(1024);
  LONGS_EQUAL(70 + 2 * 1024 + 6 * MEM_HEADER_SIZE, DMem_GetAllocSize());
}

TEST(Memory, AllocateZeroNum)
{
  DMem_Malloc(0);
  LONGS_EQUAL(0, DMem_GetAllocSize());
  LONGS_EQUAL(DMEM_HEAP, DMem_GetFreeSize());
}

TEST(Memory, AllocateTooMush){LONGS_EQUAL(NULL, DMem_Malloc(DMEM_HEAP))}

TEST(Memory, AllocateExhausted)
{
  DMem_Malloc(DMEM_HEAP - MEM_HEADER_SIZE);
  LONGS_EQUAL(DMEM_HEAP, DMem_GetAllocSize());
  LONGS_EQUAL(0, DMem_GetFreeSize());
}

TEST(Memory, ReAllocate)
{
  void *alloctated = DMem_Malloc(DMEM_HEAP - MEM_HEADER_SIZE);
  DMem_Free(alloctated);

  DMem_Malloc(DMEM_HEAP - MEM_HEADER_SIZE);

  LONGS_EQUAL(DMEM_HEAP, DMem_GetAllocSize());
  LONGS_EQUAL(0, DMem_GetFreeSize());
}

TEST(Memory, Free)
{
  void *alloctated = DMem_Malloc(10);
  DMem_Free(alloctated);
  LONGS_EQUAL(0, DMem_GetAllocSize());
  LONGS_EQUAL(DMEM_HEAP, DMem_GetFreeSize());
}

TEST(Memory, FreeAnonymousPointer)
{
  static MemHdr_t header;
  header.Len = 10;
  DMem_Free((void *)(&header + 1));
  LONGS_EQUAL(0, DMem_GetAllocSize());
  LONGS_EQUAL(DMEM_HEAP, DMem_GetFreeSize());
}

TEST(Memory, CoalesceOnce)
{
  void *alloctated1 = DMem_Malloc(10);
  void *alloctated2 = DMem_Malloc(10);
  DMem_Free(alloctated1);
  DMem_Free(alloctated2);

  void *alloctated3 = DMem_Malloc(20);

  LONGS_EQUAL(20 + MEM_HEADER_SIZE, DMem_GetAllocSize());
  LONGS_EQUAL(DMEM_HEAP - 20 - MEM_HEADER_SIZE, DMem_GetFreeSize());
  POINTERS_EQUAL(alloctated3, alloctated1);
}

TEST(Memory, CoalesceMultitimes)
{
  void *alloctated1 = DMem_Malloc(10);
  void *alloctated2 = DMem_Malloc(10);
  void *alloctated3 = DMem_Malloc(10);
  void *alloctated4 = DMem_Malloc(30);

  DMem_Free(alloctated1);
  DMem_Free(alloctated3);
  DMem_Free(alloctated2); // Coalesce 1st 2nd 3rd blocks when freeing 2nd block
  DMem_Free(alloctated4);

  void *alloctated5 = DMem_Malloc(30);

  LONGS_EQUAL(30 + MEM_HEADER_SIZE, DMem_GetAllocSize());
  LONGS_EQUAL(DMEM_HEAP - 30 - MEM_HEADER_SIZE, DMem_GetFreeSize());
  POINTERS_EQUAL(alloctated5, alloctated1);
}