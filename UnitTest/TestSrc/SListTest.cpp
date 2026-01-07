#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport_c.h"

#include "SList.h"
#include "TestReset.h"
#include "d_memb.h"

typedef struct UpdateEntry
{
  SList_t list;
  u32     updateNo;
} UpdateEntry_t;

SLIST(ObjectUpdateList);
DMEMB(ObjectUpdateMem, struct UpdateEntry, 100);

TEST_GROUP(SList){TEST_SETUP(){SList_Init(&ObjectUpdateList);
DMemb_Init(&ObjectUpdateMem);
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
  return ContainerOf(slist, UpdateEntry_t, list);
}

static UpdateEntry_t *ChopOne(void)
{
  SList_t *slist = SList_Chop(&ObjectUpdateList);
  return ContainerOf(slist, UpdateEntry_t, list);
}

TEST(SList, PushPop)
{
  PushOne(1);
  PushOne(2);

  LONGS_EQUAL(2, PopOne()->updateNo);
  LONGS_EQUAL(1, PopOne()->updateNo);
}

TEST(SList, PushChop)
{
  PushOne(1);
  PushOne(2);

  LONGS_EQUAL(1, ChopOne()->updateNo);
  LONGS_EQUAL(2, ChopOne()->updateNo);
}

TEST(SList, AddPop)
{
  AddOne(1);
  AddOne(2);

  LONGS_EQUAL(1, PopOne()->updateNo);
  LONGS_EQUAL(2, PopOne()->updateNo);
}

TEST(SList, AddChop)
{
  AddOne(1);
  AddOne(2);

  LONGS_EQUAL(2, ChopOne()->updateNo);
  LONGS_EQUAL(1, ChopOne()->updateNo);
}

TEST(SList, AddEmptyOne)
{
  SList_Add(&ObjectUpdateList, NULL);
  LONGS_EQUAL(0, SList_Len(&ObjectUpdateList));
}

TEST(SList, PushEmptyOne)
{
  SList_Push(&ObjectUpdateList, NULL);
  LONGS_EQUAL(0, SList_Len(&ObjectUpdateList));
}

TEST(SList, Length)
{
  AddOne(1);
  LONGS_EQUAL(1, SList_Len(&ObjectUpdateList));

  AddOne(2);
  LONGS_EQUAL(2, SList_Len(&ObjectUpdateList));

  ChopOne();
  LONGS_EQUAL(1, SList_Len(&ObjectUpdateList));

  ChopOne();
  LONGS_EQUAL(0, SList_Len(&ObjectUpdateList));
}

TEST(SList, Iteration)
{
  AddOne(1);
  AddOne(1);
  LONGS_EQUAL(2, SList_Len(&ObjectUpdateList));
}

TEST(SList, AddRemove)
{
  UpdateEntry_t *e = AddOne(1);
  LONGS_EQUAL(1, SList_Len(&ObjectUpdateList));
  SList_Remove(&ObjectUpdateList, &e->list);
  LONGS_EQUAL(0, SList_Len(&ObjectUpdateList));
}