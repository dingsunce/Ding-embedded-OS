#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport_c.h"

#include "TestReset.h"
#include "d_memb.h"
#include "s_list.h"

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
  LIST_HEADER;
  u32 updateNo;
} ObjectUpdateEntry_t;

LIST(ObjectUpdateList);
DMEMB(ObjectUpdateMem, struct ObjectUpdateEntry, 100);

static void PushOne(u16 no)
{
  ObjectUpdateEntry_t *e;
  e = (ObjectUpdateEntry_t *)DMemb_Alloc(&ObjectUpdateMem);
  e->updateNo = no;
  List_Push(ObjectUpdateList, e);
}

static void AddOne(u16 no)
{
  ObjectUpdateEntry_t *e;
  e = (ObjectUpdateEntry_t *)DMemb_Alloc(&ObjectUpdateMem);
  e->updateNo = no;
  List_Add(ObjectUpdateList, e);
}

static ObjectUpdateEntry_t *PopOne(void)
{
  return (ObjectUpdateEntry_t *)List_Pop(ObjectUpdateList);
}

static ObjectUpdateEntry_t *ChopOne(void)
{
  return (ObjectUpdateEntry_t *)List_Chop(ObjectUpdateList);
}

TEST(List, PushPop)
{
  List_Init(ObjectUpdateList);
  DMemb_Init(&ObjectUpdateMem);

  PushOne(1);
  PushOne(2);

  LONGS_EQUAL(2, PopOne()->updateNo);
  LONGS_EQUAL(1, PopOne()->updateNo);
}

TEST(List, PushChop)
{
  List_Init(ObjectUpdateList);
  DMemb_Init(&ObjectUpdateMem);

  PushOne(1);
  PushOne(2);

  LONGS_EQUAL(1, ChopOne()->updateNo);
  LONGS_EQUAL(2, ChopOne()->updateNo);
}

TEST(List, AddPop)
{
  List_Init(ObjectUpdateList);
  DMemb_Init(&ObjectUpdateMem);

  AddOne(1);
  AddOne(2);

  LONGS_EQUAL(1, PopOne()->updateNo);
  LONGS_EQUAL(2, PopOne()->updateNo);
}

TEST(List, AddChop)
{
  List_Init(ObjectUpdateList);
  DMemb_Init(&ObjectUpdateMem);

  AddOne(1);
  AddOne(2);

  LONGS_EQUAL(2, ChopOne()->updateNo);
  LONGS_EQUAL(1, ChopOne()->updateNo);
}

TEST(List, AddEmptyOne)
{
  List_Init(ObjectUpdateList);
  DMemb_Init(&ObjectUpdateMem);

  List_Add(ObjectUpdateList, NULL);

  LONGS_EQUAL(0, List_Length(ObjectUpdateList));
}

TEST(List, PushEmptyOne)
{
  List_Init(ObjectUpdateList);
  DMemb_Init(&ObjectUpdateMem);

  List_Push(ObjectUpdateList, NULL);

  LONGS_EQUAL(0, List_Length(ObjectUpdateList));
}

TEST(List, Length)
{
  List_Init(ObjectUpdateList);
  DMemb_Init(&ObjectUpdateMem);

  AddOne(1);
  LONGS_EQUAL(1, List_Length(ObjectUpdateList));

  AddOne(2);
  LONGS_EQUAL(2, List_Length(ObjectUpdateList));

  ChopOne();
  LONGS_EQUAL(1, List_Length(ObjectUpdateList));

  ChopOne();
  LONGS_EQUAL(0, List_Length(ObjectUpdateList));
}

static int GetLengthByIteration()
{
  ObjectUpdateEntry_t *e;
  int                  length = 0;
  for (e = (ObjectUpdateEntry_t *)List_Head(ObjectUpdateList); e != NULL;
       e = (ObjectUpdateEntry_t *)List_ItemNext(e))
  {
    length++;
  }

  return length;
}

TEST(List, Iteration)
{
  List_Init(ObjectUpdateList);
  DMemb_Init(&ObjectUpdateMem);

  AddOne(1);
  AddOne(1);
  LONGS_EQUAL(2, GetLengthByIteration());
}

TEST(List, ReInit)
{
  List_Init(ObjectUpdateList);
  DMemb_Init(&ObjectUpdateMem);

  AddOne(1);
  AddOne(1);

  List_Init(ObjectUpdateList);
  DMemb_Init(&ObjectUpdateMem);

  LONGS_EQUAL(0, List_Length(ObjectUpdateList));
  LONGS_EQUAL(100, DMemb_NumFree(&ObjectUpdateMem))
}
