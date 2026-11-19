#include <gtest/gtest.h>

#include "TestReset.h"
#include "d_memb.h"

typedef struct MembTest
{
  u8 Id;
} MembTest_t;

DMEMB(MembTestMem, MembTest_t, 10);

class MemoryBlock : public ::testing::Test
{
protected:
  void SetUp() override
  {
    DMemb_Init(&MembTestMem);
  }

  void TearDown() override
  {
    TestReset();
  }
};

TEST_F(MemoryBlock, AllocateAndFree)
{
  void *ptr0 = DMemb_Alloc(&MembTestMem);
  void *ptr1 = DMemb_Alloc(&MembTestMem);
  EXPECT_TRUE(DMemb_InMem(&MembTestMem, ptr0));
  EXPECT_TRUE(DMemb_InMem(&MembTestMem, ptr1));
  EXPECT_EQ(8, DMemb_NumFree(&MembTestMem));

  DMemb_Free(&MembTestMem, ptr0);
  DMemb_Free(&MembTestMem, ptr1);
  EXPECT_FALSE(DMemb_InMem(&MembTestMem, ptr0));
  EXPECT_FALSE(DMemb_InMem(&MembTestMem, ptr1));
  EXPECT_EQ(10, DMemb_NumFree(&MembTestMem));
}
