#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport_c.h"

#include "TestReset.h"
#include "d_list.h"
#include "d_memb.h"

#include "d_process.h"

PROCESS(Progress1);
PROCESS(Progress2);
PROCESS(Progress3);
DOUBLE_LIST(ProcessList);
DOUBLE_LIST(ProcessListAppend);

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
  DoubleList_Init(&ProcessList);
  TestReset();
  mock_c()->checkExpectations();
  mock_c()->clear();
  TestOutput::disableCompactFormat();
}
}
;

TEST(DList, Init)
{
  LONGS_EQUAL(0, DoubleList_Len(&ProcessList));
  CHECK_TRUE(DoubleList_IsEmpty(&ProcessList));
  POINTERS_EQUAL(NULL, DoubleList_Pop(&ProcessList))
}

TEST(DList, PushPop)
{
  DoubleList_Push(&ProcessList, &Progress1.ProcessList);
  DoubleList_Push(&ProcessList, &Progress2.ProcessList);

  LONGS_EQUAL(2, DoubleList_Len(&ProcessList));
  CHECK_FALSE(DoubleList_IsEmpty(&ProcessList));

  Double_List_t *l1 = DoubleList_Pop(&ProcessList);
  Double_List_t *l2 = DoubleList_Pop(&ProcessList);
  POINTERS_EQUAL(&Progress2.ProcessList, l1);
  POINTERS_EQUAL(&Progress1.ProcessList, l2);
  POINTERS_EQUAL(&Progress2, DContainerOf(l1, DProcess_t, ProcessList));
  POINTERS_EQUAL(&Progress1, DContainerOf(l2, DProcess_t, ProcessList));
}

TEST(DList, AddPop)
{
  DoubleList_Add(&ProcessList, &Progress1.ProcessList);
  DoubleList_Add(&ProcessList, &Progress2.ProcessList);

  LONGS_EQUAL(2, DoubleList_Len(&ProcessList));
  CHECK_FALSE(DoubleList_IsEmpty(&ProcessList));

  Double_List_t *l1 = DoubleList_Pop(&ProcessList);
  Double_List_t *l2 = DoubleList_Pop(&ProcessList);
  POINTERS_EQUAL(&Progress1.ProcessList, l1);
  POINTERS_EQUAL(&Progress2.ProcessList, l2);
  POINTERS_EQUAL(&Progress1, DContainerOf(l1, DProcess_t, ProcessList));
  POINTERS_EQUAL(&Progress2, DContainerOf(l2, DProcess_t, ProcessList));
  ;
}

TEST(DList, Remove)
{
  DoubleList_Push(&ProcessList, &Progress1.ProcessList);
  DoubleList_Push(&ProcessList, &Progress2.ProcessList);
  DoubleList_Remove(&Progress1.ProcessList);
  DoubleList_Remove(&Progress2.ProcessList);

  LONGS_EQUAL(0, DoubleList_Len(&ProcessList));
  CHECK_TRUE(DoubleList_IsEmpty(&ProcessList));
  POINTERS_EQUAL(NULL, DoubleList_Pop(&ProcessList))
}

TEST(DList, ReInit)
{
  DoubleList_Push(&ProcessList, &Progress1.ProcessList);
  DoubleList_Push(&ProcessList, &Progress2.ProcessList);
  DoubleList_Init(&ProcessList);

  LONGS_EQUAL(0, DoubleList_Len(&ProcessList));
  CHECK_TRUE(DoubleList_IsEmpty(&ProcessList));
  POINTERS_EQUAL(NULL, DoubleList_Pop(&ProcessList))
}

TEST(DList, Spilce)
{
  DoubleList_Push(&ProcessList, &Progress1.ProcessList);
  DoubleList_Push(&ProcessList, &Progress2.ProcessList);
  DoubleList_Push(&ProcessListAppend, &Progress3.ProcessList);
  DoubleList_Splice(&ProcessList, &ProcessListAppend);
  LONGS_EQUAL(3, DoubleList_Len(&ProcessList));
}
