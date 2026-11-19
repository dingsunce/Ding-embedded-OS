#include <gtest/gtest.h>

#include "TestReset.h"
#include "d_memb.h"
#include "s_list.h"

typedef struct UpdateEntry
{
  SList_t list;
  u32     updateNo;
} UpdateEntry_t;

SLIST(ObjectUpdateList);
DMEMB(ObjectUpdateMem, struct UpdateEntry, 100);

class SListTestFixture : public ::testing::Test
{
protected:
  void SetUp() override
  {
    SList_Init(&ObjectUpdateList);
    DMemb_Init(&ObjectUpdateMem);
  }

  void TearDown() override
  {
    TestReset();
  }
};

static void PushOne(u16 no)
{
  UpdateEntry_t *e;
  e = (UpdateEntry_t *)DMemb_Alloc(&ObjectUpdateMem);
  e->updateNo = no;
  SList_Push(&ObjectUpdateList, &e->list);
}

static UpdateEntry_t *AddOne(u16 no)
{
  UpdateEntry_t *e;
  e = (UpdateEntry_t *)DMemb_Alloc(&ObjectUpdateMem);
  e->updateNo = no;
  SList_Add(&ObjectUpdateList, &e->list);
  return e;
}

static UpdateEntry_t *PopOne(void)
{
  SList_t *slist = SList_Pop(&ObjectUpdateList);
  return DContainerOf(slist, UpdateEntry_t, list);
}

static UpdateEntry_t *ChopOne(void)
{
  SList_t *slist = SList_Chop(&ObjectUpdateList);
  return DContainerOf(slist, UpdateEntry_t, list);
}

TEST_F(SListTestFixture, PushPop)
{
  PushOne(1);
  PushOne(2);

  EXPECT_EQ(2, PopOne()->updateNo);
  EXPECT_EQ(1, PopOne()->updateNo);
}

TEST_F(SListTestFixture, PushChop)
{
  PushOne(1);
  PushOne(2);

  EXPECT_EQ(1, ChopOne()->updateNo);
  EXPECT_EQ(2, ChopOne()->updateNo);
}

TEST_F(SListTestFixture, AddPop)
{
  AddOne(1);
  AddOne(2);

  EXPECT_EQ(1, PopOne()->updateNo);
  EXPECT_EQ(2, PopOne()->updateNo);
}

TEST_F(SListTestFixture, AddChop)
{
  AddOne(1);
  AddOne(2);

  EXPECT_EQ(2, ChopOne()->updateNo);
  EXPECT_EQ(1, ChopOne()->updateNo);
}

TEST_F(SListTestFixture, AddEmptyOne)
{
  SList_Add(&ObjectUpdateList, NULL);
  EXPECT_EQ(0, SList_Len(&ObjectUpdateList));
}

TEST_F(SListTestFixture, PushEmptyOne)
{
  SList_Push(&ObjectUpdateList, NULL);
  EXPECT_EQ(0, SList_Len(&ObjectUpdateList));
}

TEST_F(SListTestFixture, Length)
{
  AddOne(1);
  EXPECT_EQ(1, SList_Len(&ObjectUpdateList));

  AddOne(2);
  EXPECT_EQ(2, SList_Len(&ObjectUpdateList));

  ChopOne();
  EXPECT_EQ(1, SList_Len(&ObjectUpdateList));

  ChopOne();
  EXPECT_EQ(0, SList_Len(&ObjectUpdateList));
}

TEST_F(SListTestFixture, Iteration)
{
  AddOne(1);
  AddOne(1);
  EXPECT_EQ(2, SList_Len(&ObjectUpdateList));
}

TEST_F(SListTestFixture, AddRemove)
{
  UpdateEntry_t *e = AddOne(1);
  EXPECT_EQ(1, SList_Len(&ObjectUpdateList));
  SList_Remove(&ObjectUpdateList, &e->list);
  EXPECT_EQ(0, SList_Len(&ObjectUpdateList));
}

TEST_F(SListTestFixture, RemoveFromBeginning)
{
  UpdateEntry_t *e1 = AddOne(1);
  UpdateEntry_t *e2 = AddOne(2);
  UpdateEntry_t *e3 = AddOne(3);
  EXPECT_EQ(3, SList_Len(&ObjectUpdateList));

  SList_Remove(&ObjectUpdateList, &e1->list);
  EXPECT_EQ(2, SList_Len(&ObjectUpdateList));
  EXPECT_EQ(2, PopOne()->updateNo);
  EXPECT_EQ(3, PopOne()->updateNo);
}

TEST_F(SListTestFixture, RemoveFromMiddle)
{
  UpdateEntry_t *e1 = AddOne(1);
  UpdateEntry_t *e2 = AddOne(2);
  UpdateEntry_t *e3 = AddOne(3);
  EXPECT_EQ(3, SList_Len(&ObjectUpdateList));

  SList_Remove(&ObjectUpdateList, &e2->list);
  EXPECT_EQ(2, SList_Len(&ObjectUpdateList));
  EXPECT_EQ(1, PopOne()->updateNo);
  EXPECT_EQ(3, PopOne()->updateNo);
}

TEST_F(SListTestFixture, RemoveFromEnd)
{
  UpdateEntry_t *e1 = AddOne(1);
  UpdateEntry_t *e2 = AddOne(2);
  UpdateEntry_t *e3 = AddOne(3);
  EXPECT_EQ(3, SList_Len(&ObjectUpdateList));

  SList_Remove(&ObjectUpdateList, &e3->list);
  EXPECT_EQ(2, SList_Len(&ObjectUpdateList));
  EXPECT_EQ(1, PopOne()->updateNo);
  EXPECT_EQ(2, PopOne()->updateNo);
}

TEST_F(SListTestFixture, RemoveNonExistentElement)
{
  UpdateEntry_t *e1 = AddOne(1);
  UpdateEntry_t *e2 = AddOne(2);
  UpdateEntry_t *e3 = (UpdateEntry_t *)DMemb_Alloc(&ObjectUpdateMem);
  e3->updateNo = 99;
  EXPECT_EQ(2, SList_Len(&ObjectUpdateList));

  SList_Remove(&ObjectUpdateList, &e3->list);
  EXPECT_EQ(2, SList_Len(&ObjectUpdateList));
  EXPECT_EQ(1, PopOne()->updateNo);
  EXPECT_EQ(2, PopOne()->updateNo);
}

TEST_F(SListTestFixture, RemoveFromEmptyList)
{
  UpdateEntry_t *e1 = (UpdateEntry_t *)DMemb_Alloc(&ObjectUpdateMem);
  e1->updateNo = 1;
  EXPECT_EQ(0, SList_Len(&ObjectUpdateList));

  SList_Remove(&ObjectUpdateList, &e1->list);
  EXPECT_EQ(0, SList_Len(&ObjectUpdateList));
}
