#include <gtest/gtest.h>

#include "TestReset.h"
#include "d_list.h"
#include "d_memb.h"

class List : public ::testing::Test
{
protected:
  void TearDown() override
  {
    TestReset();
  }
};

typedef struct ObjectUpdateEntry
{
  DLIST_HEADER;
  u32 updateNo;
} ObjectUpdateEntry_t;

DLIST(ObjectUpdateList);
DMEMB(ObjectUpdateMem, struct ObjectUpdateEntry, 100);

static void PushOne(u16 no)
{
  ObjectUpdateEntry_t *e;
  e = (ObjectUpdateEntry_t *)DMemb_Alloc(&ObjectUpdateMem);
  e->updateNo = no;
  DList_Push(ObjectUpdateList, e);
}

static void AddOne(u16 no)
{
  ObjectUpdateEntry_t *e;
  e = (ObjectUpdateEntry_t *)DMemb_Alloc(&ObjectUpdateMem);
  e->updateNo = no;
  DList_Add(ObjectUpdateList, e);
}

static ObjectUpdateEntry_t *PopOne(void)
{
  return (ObjectUpdateEntry_t *)DList_Pop(ObjectUpdateList);
}

static ObjectUpdateEntry_t *ChopOne(void)
{
  return (ObjectUpdateEntry_t *)DList_Chop(ObjectUpdateList);
}

static void InsertOne(u16 no, void *prevItem)
{
  ObjectUpdateEntry_t *e;
  e = (ObjectUpdateEntry_t *)DMemb_Alloc(&ObjectUpdateMem);
  e->updateNo = no;
  DList_Insert(ObjectUpdateList, prevItem, e);
}

TEST_F(List, PushPop)
{
  DList_Init(ObjectUpdateList);
  DMemb_Init(&ObjectUpdateMem);

  PushOne(1);
  PushOne(2);

  EXPECT_EQ(2, PopOne()->updateNo);
  EXPECT_EQ(1, PopOne()->updateNo);
}

TEST_F(List, PushChop)
{
  DList_Init(ObjectUpdateList);
  DMemb_Init(&ObjectUpdateMem);

  PushOne(1);
  PushOne(2);

  EXPECT_EQ(1, ChopOne()->updateNo);
  EXPECT_EQ(2, ChopOne()->updateNo);
}

TEST_F(List, AddPop)
{
  DList_Init(ObjectUpdateList);
  DMemb_Init(&ObjectUpdateMem);

  AddOne(1);
  AddOne(2);

  EXPECT_EQ(1, PopOne()->updateNo);
  EXPECT_EQ(2, PopOne()->updateNo);
}

TEST_F(List, AddChop)
{
  DList_Init(ObjectUpdateList);
  DMemb_Init(&ObjectUpdateMem);

  AddOne(1);
  AddOne(2);

  EXPECT_EQ(2, ChopOne()->updateNo);
  EXPECT_EQ(1, ChopOne()->updateNo);
}

TEST_F(List, AddEmptyOne)
{
  DList_Init(ObjectUpdateList);
  DMemb_Init(&ObjectUpdateMem);

  DList_Add(ObjectUpdateList, NULL);

  EXPECT_EQ(0, DList_Length(ObjectUpdateList));
}

TEST_F(List, PushEmptyOne)
{
  DList_Init(ObjectUpdateList);
  DMemb_Init(&ObjectUpdateMem);

  DList_Push(ObjectUpdateList, NULL);

  EXPECT_EQ(0, DList_Length(ObjectUpdateList));
}

TEST_F(List, IsEmpty)
{
  DList_Init(ObjectUpdateList);
  DMemb_Init(&ObjectUpdateMem);

  EXPECT_TRUE(DList_IsEmpty(ObjectUpdateList));

  AddOne(1);
  EXPECT_FALSE(DList_IsEmpty(ObjectUpdateList));

  PopOne();
  EXPECT_TRUE(DList_IsEmpty(ObjectUpdateList));
}

TEST_F(List, Length)
{
  DList_Init(ObjectUpdateList);
  DMemb_Init(&ObjectUpdateMem);

  AddOne(1);
  EXPECT_EQ(1, DList_Length(ObjectUpdateList));

  AddOne(2);
  EXPECT_EQ(2, DList_Length(ObjectUpdateList));

  ChopOne();
  EXPECT_EQ(1, DList_Length(ObjectUpdateList));

  ChopOne();
  EXPECT_EQ(0, DList_Length(ObjectUpdateList));
}

static int GetLengthByIteration()
{
  ObjectUpdateEntry_t *e;
  int                  length = 0;
  for (e = (ObjectUpdateEntry_t *)DList_Head(ObjectUpdateList); e != NULL;
       e = (ObjectUpdateEntry_t *)DList_ItemNext(e))
  {
    length++;
  }

  return length;
}

TEST_F(List, Iteration)
{
  DList_Init(ObjectUpdateList);
  DMemb_Init(&ObjectUpdateMem);

  AddOne(1);
  AddOne(1);
  EXPECT_EQ(2, GetLengthByIteration());
}

TEST_F(List, ReInit)
{
  DList_Init(ObjectUpdateList);
  DMemb_Init(&ObjectUpdateMem);

  AddOne(1);
  AddOne(1);

  DList_Init(ObjectUpdateList);
  DMemb_Init(&ObjectUpdateMem);

  EXPECT_EQ(0, DList_Length(ObjectUpdateList));
  EXPECT_EQ(100, DMemb_NumFree(&ObjectUpdateMem));
}

TEST_F(List, Insert)
{
  DList_Init(ObjectUpdateList);
  DMemb_Init(&ObjectUpdateMem);

  // Insert into empty list (should push to front)
  InsertOne(1, NULL);
  EXPECT_EQ(1, DList_Length(ObjectUpdateList));
  EXPECT_EQ(1, PopOne()->updateNo);

  // Insert after first item
  AddOne(1); // list: 1
  AddOne(2); // list: 1 -> 2
  ObjectUpdateEntry_t *first = (ObjectUpdateEntry_t *)DList_Head(ObjectUpdateList);
  InsertOne(3, first); // insert 3 after 1: 1 -> 3 -> 2
  EXPECT_EQ(3, DList_Length(ObjectUpdateList));
  EXPECT_EQ(1, PopOne()->updateNo);
  EXPECT_EQ(3, PopOne()->updateNo);
  EXPECT_EQ(2, PopOne()->updateNo);

  // Insert at end
  DList_Init(ObjectUpdateList); // reset list
  DMemb_Init(&ObjectUpdateMem);
  AddOne(1); // list: 1
  AddOne(2); // list: 1 -> 2
  ObjectUpdateEntry_t *second = (ObjectUpdateEntry_t *)DList_ItemNext(DList_Head(ObjectUpdateList));
  InsertOne(4, second); // insert 4 after 2: 1 -> 2 -> 4
  EXPECT_EQ(3, DList_Length(ObjectUpdateList));
  EXPECT_EQ(1, PopOne()->updateNo);
  EXPECT_EQ(2, PopOne()->updateNo);
  EXPECT_EQ(4, PopOne()->updateNo);
}
