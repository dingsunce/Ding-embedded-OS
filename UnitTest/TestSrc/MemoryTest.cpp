#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport_c.h"

#include "TestReset.h"
#include "memory.h"

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
  Mem_Malloc(10);
  LONGS_EQUAL(10 + MEM_HEADER_SIZE, Mem_GetAllocSize());
  LONGS_EQUAL(MEM_HEAP - 10 - MEM_HEADER_SIZE, Mem_GetFreeSize());
}

TEST(Memory, AllocateZeroNum)
{
  Mem_Malloc(0);
  LONGS_EQUAL(0, Mem_GetAllocSize());
  LONGS_EQUAL(MEM_HEAP, Mem_GetFreeSize());
}

TEST(Memory, AllocateTooMush){LONGS_EQUAL(NULL, Mem_Malloc(MEM_HEAP))}

TEST(Memory, AllocateExhausted)
{
  Mem_Malloc(MEM_HEAP - MEM_HEADER_SIZE);
  LONGS_EQUAL(MEM_HEAP, Mem_GetAllocSize());
  LONGS_EQUAL(0, Mem_GetFreeSize());
}

TEST(Memory, ReAllocate)
{
  void* alloctated = Mem_Malloc(MEM_HEAP - MEM_HEADER_SIZE);
  Mem_Free(alloctated);

  Mem_Malloc(MEM_HEAP - MEM_HEADER_SIZE);

  LONGS_EQUAL(MEM_HEAP, Mem_GetAllocSize());
  LONGS_EQUAL(0, Mem_GetFreeSize());
}

TEST(Memory, Free)
{
  void* alloctated = Mem_Malloc(10);
  Mem_Free(alloctated);
  LONGS_EQUAL(0, Mem_GetAllocSize());
  LONGS_EQUAL(MEM_HEAP, Mem_GetFreeSize());
}

TEST(Memory, FreeAnonymousPointer)
{
  static MemHdr_t header;
  header.Len = 10;
  Mem_Free((void*)(&header + 1));
  LONGS_EQUAL(0, Mem_GetAllocSize());
  LONGS_EQUAL(MEM_HEAP, Mem_GetFreeSize());
}

TEST(Memory, CoalesceOnce)
{
  void* alloctated1 = Mem_Malloc(10);
  void* alloctated2 = Mem_Malloc(10);
  Mem_Free(alloctated1);
  Mem_Free(alloctated2);

  void* alloctated3 = Mem_Malloc(20);

  LONGS_EQUAL(20 + MEM_HEADER_SIZE, Mem_GetAllocSize());
  LONGS_EQUAL(MEM_HEAP - 20 - MEM_HEADER_SIZE, Mem_GetFreeSize());
  POINTERS_EQUAL(alloctated3, alloctated1);
}

TEST(Memory, CoalesceMultitimes)
{
  void* alloctated1 = Mem_Malloc(10);
  void* alloctated2 = Mem_Malloc(10);
  void* alloctated3 = Mem_Malloc(10);
  void* alloctated4 = Mem_Malloc(30);

  Mem_Free(alloctated1);
  Mem_Free(alloctated3);
  Mem_Free(alloctated2); // Coalesce 1st 2nd 3rd blocks when freeing 2nd block
  Mem_Free(alloctated4);

  void* alloctated5 = Mem_Malloc(30);

  LONGS_EQUAL(30 + MEM_HEADER_SIZE, Mem_GetAllocSize());
  LONGS_EQUAL(MEM_HEAP - 30 - MEM_HEADER_SIZE, Mem_GetFreeSize());
  POINTERS_EQUAL(alloctated5, alloctated1);
}