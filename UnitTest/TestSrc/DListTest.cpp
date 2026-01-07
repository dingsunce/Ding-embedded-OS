#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport_c.h"

#include "TestReset.h"
#include "d_memb.h"
#include "s_list.h"

#include "process.h"

PROCESS(Progress1);
PROCESS(Progress2);
PROCESS(Progress3);
DLIST(ProcessList);
DLIST(ProcessListAppend);

PROCESS_HANDLER(Progress1, msgId, arg)
{
  PROCESS_SCHEDULE_BEGIN();

  PROCESS_SCHEDULE_END();
}

PROCESS_HANDLER(Progress2, msgId, arg)
{
  PROCESS_SCHEDULE_BEGIN();

  PROCESS_SCHEDULE_END();
}

PROCESS_HANDLER(Progress3, msgId, arg)
{
  PROCESS_SCHEDULE_BEGIN();

  PROCESS_SCHEDULE_END();
}

TEST_GROUP(DList){TEST_SETUP(){TestOutput::enableCompactFormat();
}

TEST_TEARDOWN()
{
  DList_Init(&ProcessList);
  TestReset();
  mock_c()->checkExpectations();
  mock_c()->clear();
  TestOutput::disableCompactFormat();
}
}
;

TEST(DList, Init)
{
  LONGS_EQUAL(0, DList_Len(&ProcessList));
  CHECK_TRUE(DList_IsEmpty(&ProcessList));
  POINTERS_EQUAL(NULL, DList_Pop(&ProcessList))
}

TEST(DList, PushPop)
{
  DList_Push(&ProcessList, &Progress1.ProcessList);
  DList_Push(&ProcessList, &Progress2.ProcessList);

  LONGS_EQUAL(2, DList_Len(&ProcessList));
  CHECK_FALSE(DList_IsEmpty(&ProcessList));

  DList_t *l1 = DList_Pop(&ProcessList);
  DList_t *l2 = DList_Pop(&ProcessList);
  POINTERS_EQUAL(&Progress2.ProcessList, l1);
  POINTERS_EQUAL(&Progress1.ProcessList, l2);
  POINTERS_EQUAL(&Progress2, ContainerOf(l1, Process_t, ProcessList));
  POINTERS_EQUAL(&Progress1, ContainerOf(l2, Process_t, ProcessList));
}

TEST(DList, AddPop)
{
  DList_Add(&ProcessList, &Progress1.ProcessList);
  DList_Add(&ProcessList, &Progress2.ProcessList);

  LONGS_EQUAL(2, DList_Len(&ProcessList));
  CHECK_FALSE(DList_IsEmpty(&ProcessList));

  DList_t *l1 = DList_Pop(&ProcessList);
  DList_t *l2 = DList_Pop(&ProcessList);
  POINTERS_EQUAL(&Progress1.ProcessList, l1);
  POINTERS_EQUAL(&Progress2.ProcessList, l2);
  POINTERS_EQUAL(&Progress1, ContainerOf(l1, Process_t, ProcessList));
  POINTERS_EQUAL(&Progress2, ContainerOf(l2, Process_t, ProcessList));
  ;
}

TEST(DList, Remove)
{
  DList_Push(&ProcessList, &Progress1.ProcessList);
  DList_Push(&ProcessList, &Progress2.ProcessList);
  DList_Remove(&Progress1.ProcessList);
  DList_Remove(&Progress2.ProcessList);

  LONGS_EQUAL(0, DList_Len(&ProcessList));
  CHECK_TRUE(DList_IsEmpty(&ProcessList));
  POINTERS_EQUAL(NULL, DList_Pop(&ProcessList))
}

TEST(DList, ReInit)
{
  DList_Push(&ProcessList, &Progress1.ProcessList);
  DList_Push(&ProcessList, &Progress2.ProcessList);
  DList_Init(&ProcessList);

  LONGS_EQUAL(0, DList_Len(&ProcessList));
  CHECK_TRUE(DList_IsEmpty(&ProcessList));
  POINTERS_EQUAL(NULL, DList_Pop(&ProcessList))
}

TEST(DList, Spilce)
{
  DList_Push(&ProcessList, &Progress1.ProcessList);
  DList_Push(&ProcessList, &Progress2.ProcessList);
  DList_Push(&ProcessListAppend, &Progress3.ProcessList);
  DList_Splice(&ProcessList, &ProcessListAppend);
  LONGS_EQUAL(3, DList_Len(&ProcessList));
}
