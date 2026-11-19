#include <gtest/gtest.h>

#include "TestReset.h"
#include "d_mem.h"

class Memory : public ::testing::Test
{
protected:
  void TearDown() override
  {
    TestReset();
  }
};

TEST_F(Memory, Allocate)
{
  DMem_Malloc(10);
  EXPECT_EQ(10 + MEM_HEADER_SIZE, DMem_GetAllocSize());
  EXPECT_EQ(DMEM_HEAP - 10 - MEM_HEADER_SIZE, DMem_GetFreeSize());

  DMem_Malloc(20);
  EXPECT_EQ(30 + 2 * MEM_HEADER_SIZE, DMem_GetAllocSize());

  DMem_Malloc(20);
  EXPECT_EQ(50 + 3 * MEM_HEADER_SIZE, DMem_GetAllocSize());

  DMem_Malloc(20);
  EXPECT_EQ(70 + 4 * MEM_HEADER_SIZE, DMem_GetAllocSize());

  DMem_Malloc(1024);
  EXPECT_EQ(70 + 1024 + 5 * MEM_HEADER_SIZE, DMem_GetAllocSize());

  DMem_Malloc(1024);
  EXPECT_EQ(70 + 2 * 1024 + 6 * MEM_HEADER_SIZE, DMem_GetAllocSize());
}

TEST_F(Memory, AllocateZeroNum)
{
  DMem_Malloc(0);
  EXPECT_EQ(0, DMem_GetAllocSize());
  EXPECT_EQ(DMEM_HEAP, DMem_GetFreeSize());
}

TEST_F(Memory, AllocateTooMush)
{
  EXPECT_EQ(nullptr, DMem_Malloc(DMEM_HEAP));
}

TEST_F(Memory, AllocateExhausted)
{
  DMem_Malloc(DMEM_HEAP - MEM_HEADER_SIZE);
  EXPECT_EQ(DMEM_HEAP, DMem_GetAllocSize());
  EXPECT_EQ(0, DMem_GetFreeSize());
}

TEST_F(Memory, ReAllocate)
{
  void *alloctated = DMem_Malloc(DMEM_HEAP - MEM_HEADER_SIZE);
  DMem_Free(alloctated);

  DMem_Malloc(DMEM_HEAP - MEM_HEADER_SIZE);

  EXPECT_EQ(DMEM_HEAP, DMem_GetAllocSize());
  EXPECT_EQ(0, DMem_GetFreeSize());
}

TEST_F(Memory, Free)
{
  void *alloctated = DMem_Malloc(10);
  DMem_Free(alloctated);
  EXPECT_EQ(0, DMem_GetAllocSize());
  EXPECT_EQ(DMEM_HEAP, DMem_GetFreeSize());
}

TEST_F(Memory, FreeAnonymousPointer)
{
  static MemHdr_t header;
  header.Len = 10;
  DMem_Free((void *)(&header + 1));
  EXPECT_EQ(0, DMem_GetAllocSize());
  EXPECT_EQ(DMEM_HEAP, DMem_GetFreeSize());
}

TEST_F(Memory, CoalesceOnce)
{
  void *alloctated1 = DMem_Malloc(10);
  void *alloctated2 = DMem_Malloc(10);
  DMem_Free(alloctated1);
  DMem_Free(alloctated2);

  void *alloctated3 = DMem_Malloc(20);

  EXPECT_EQ(20 + MEM_HEADER_SIZE, DMem_GetAllocSize());
  EXPECT_EQ(DMEM_HEAP - 20 - MEM_HEADER_SIZE, DMem_GetFreeSize());
  EXPECT_EQ(alloctated3, alloctated1);
}

TEST_F(Memory, CoalesceMultitimes)
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

  EXPECT_EQ(30 + MEM_HEADER_SIZE, DMem_GetAllocSize());
  EXPECT_EQ(DMEM_HEAP - 30 - MEM_HEADER_SIZE, DMem_GetFreeSize());
  EXPECT_EQ(alloctated5, alloctated1);
}
