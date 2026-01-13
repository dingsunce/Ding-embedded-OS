#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport_c.h"

#include "TestReset.h"
#include "d_list.h"
#include "d_memb.h"

TEST_GROUP(List){TEST_SETUP(){TestOutput::enableCompactFormat();
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

TEST(List, PushPop)
{
  DList_Init(ObjectUpdateList);
  DMemb_Init(&ObjectUpdateMem);

  PushOne(1);
  PushOne(2);

  LONGS_EQUAL(2, PopOne()->updateNo);
  LONGS_EQUAL(1, PopOne()->updateNo);
}

TEST(List, PushChop)
{
  DList_Init(ObjectUpdateList);
  DMemb_Init(&ObjectUpdateMem);

  PushOne(1);
  PushOne(2);

  LONGS_EQUAL(1, ChopOne()->updateNo);
  LONGS_EQUAL(2, ChopOne()->updateNo);
}

TEST(List, AddPop)
{
  DList_Init(ObjectUpdateList);
  DMemb_Init(&ObjectUpdateMem);

  AddOne(1);
  AddOne(2);

  LONGS_EQUAL(1, PopOne()->updateNo);
  LONGS_EQUAL(2, PopOne()->updateNo);
}

TEST(List, AddChop)
{
  DList_Init(ObjectUpdateList);
  DMemb_Init(&ObjectUpdateMem);

  AddOne(1);
  AddOne(2);

  LONGS_EQUAL(2, ChopOne()->updateNo);
  LONGS_EQUAL(1, ChopOne()->updateNo);
}

TEST(List, AddEmptyOne)
{
  DList_Init(ObjectUpdateList);
  DMemb_Init(&ObjectUpdateMem);

  DList_Add(ObjectUpdateList, NULL);

  LONGS_EQUAL(0, DList_Length(ObjectUpdateList));
}

TEST(List, PushEmptyOne)
{
  DList_Init(ObjectUpdateList);
  DMemb_Init(&ObjectUpdateMem);

  DList_Push(ObjectUpdateList, NULL);

  LONGS_EQUAL(0, DList_Length(ObjectUpdateList));
}

TEST(List, Length)
{
  DList_Init(ObjectUpdateList);
  DMemb_Init(&ObjectUpdateMem);

  AddOne(1);
  LONGS_EQUAL(1, DList_Length(ObjectUpdateList));

  AddOne(2);
  LONGS_EQUAL(2, DList_Length(ObjectUpdateList));

  ChopOne();
  LONGS_EQUAL(1, DList_Length(ObjectUpdateList));

  ChopOne();
  LONGS_EQUAL(0, DList_Length(ObjectUpdateList));
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

TEST(List, Iteration)
{
  DList_Init(ObjectUpdateList);
  DMemb_Init(&ObjectUpdateMem);

  AddOne(1);
  AddOne(1);
  LONGS_EQUAL(2, GetLengthByIteration());
}

TEST(List, ReInit)
{
  DList_Init(ObjectUpdateList);
  DMemb_Init(&ObjectUpdateMem);

  AddOne(1);
  AddOne(1);

  DList_Init(ObjectUpdateList);
  DMemb_Init(&ObjectUpdateMem);

  LONGS_EQUAL(0, DList_Length(ObjectUpdateList));
  LONGS_EQUAL(100, DMemb_NumFree(&ObjectUpdateMem))
}

TEST(List, Insert)
{
  DList_Init(ObjectUpdateList);
  DMemb_Init(&ObjectUpdateMem);

  // Insert into empty list (should push to front)
  InsertOne(1, NULL);
  LONGS_EQUAL(1, DList_Length(ObjectUpdateList));
  LONGS_EQUAL(1, PopOne()->updateNo);

  // Insert after first item
  AddOne(1); // list: 1
  AddOne(2); // list: 1 -> 2
  ObjectUpdateEntry_t *first = (ObjectUpdateEntry_t *)DList_Head(ObjectUpdateList);
  InsertOne(3, first); // insert 3 after 1: 1 -> 3 -> 2
  LONGS_EQUAL(3, DList_Length(ObjectUpdateList));
  LONGS_EQUAL(1, PopOne()->updateNo);
  LONGS_EQUAL(3, PopOne()->updateNo);
  LONGS_EQUAL(2, PopOne()->updateNo);

  // Insert at end
  DList_Init(ObjectUpdateList); // reset list
  DMemb_Init(&ObjectUpdateMem);
  AddOne(1); // list: 1
  AddOne(2); // list: 1 -> 2
  ObjectUpdateEntry_t *second = (ObjectUpdateEntry_t *)DList_ItemNext(DList_Head(ObjectUpdateList));
  InsertOne(4, second); // insert 4 after 2: 1 -> 2 -> 4
  LONGS_EQUAL(3, DList_Length(ObjectUpdateList));
  LONGS_EQUAL(1, PopOne()->updateNo);
  LONGS_EQUAL(2, PopOne()->updateNo);
  LONGS_EQUAL(4, PopOne()->updateNo);
}
