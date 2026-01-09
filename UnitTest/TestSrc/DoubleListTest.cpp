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
DB_LIST(ProcessList);
DB_LIST(ProcessListAppend);

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
  DbList_Init(&ProcessList);
  TestReset();
  mock_c()->checkExpectations();
  mock_c()->clear();
  TestOutput::disableCompactFormat();
}
}
;

TEST(DList, Init)
{
  LONGS_EQUAL(0, DbList_Len(&ProcessList));
  CHECK_TRUE(DbList_IsEmpty(&ProcessList));
  POINTERS_EQUAL(NULL, DbList_Pop(&ProcessList))
}

TEST(DList, PushPop)
{
  DbList_Push(&ProcessList, &Progress1.ProcessList);
  DbList_Push(&ProcessList, &Progress2.ProcessList);

  LONGS_EQUAL(2, DbList_Len(&ProcessList));
  CHECK_FALSE(DbList_IsEmpty(&ProcessList));

  Db_List_t *l1 = DbList_Pop(&ProcessList);
  Db_List_t *l2 = DbList_Pop(&ProcessList);
  POINTERS_EQUAL(&Progress2.ProcessList, l1);
  POINTERS_EQUAL(&Progress1.ProcessList, l2);
  POINTERS_EQUAL(&Progress2, DContainerOf(l1, DProcess_t, ProcessList));
  POINTERS_EQUAL(&Progress1, DContainerOf(l2, DProcess_t, ProcessList));
}

TEST(DList, AddPop)
{
  DbList_Add(&ProcessList, &Progress1.ProcessList);
  DbList_Add(&ProcessList, &Progress2.ProcessList);

  LONGS_EQUAL(2, DbList_Len(&ProcessList));
  CHECK_FALSE(DbList_IsEmpty(&ProcessList));

  Db_List_t *l1 = DbList_Pop(&ProcessList);
  Db_List_t *l2 = DbList_Pop(&ProcessList);
  POINTERS_EQUAL(&Progress1.ProcessList, l1);
  POINTERS_EQUAL(&Progress2.ProcessList, l2);
  POINTERS_EQUAL(&Progress1, DContainerOf(l1, DProcess_t, ProcessList));
  POINTERS_EQUAL(&Progress2, DContainerOf(l2, DProcess_t, ProcessList));
  ;
}

TEST(DList, Remove)
{
  DbList_Push(&ProcessList, &Progress1.ProcessList);
  DbList_Push(&ProcessList, &Progress2.ProcessList);
  DbList_Remove(&Progress1.ProcessList);
  DbList_Remove(&Progress2.ProcessList);

  LONGS_EQUAL(0, DbList_Len(&ProcessList));
  CHECK_TRUE(DbList_IsEmpty(&ProcessList));
  POINTERS_EQUAL(NULL, DbList_Pop(&ProcessList))
}

TEST(DList, ReInit)
{
  DbList_Push(&ProcessList, &Progress1.ProcessList);
  DbList_Push(&ProcessList, &Progress2.ProcessList);
  DbList_Init(&ProcessList);

  LONGS_EQUAL(0, DbList_Len(&ProcessList));
  CHECK_TRUE(DbList_IsEmpty(&ProcessList));
  POINTERS_EQUAL(NULL, DbList_Pop(&ProcessList))
}

TEST(DList, Spilce)
{
  DbList_Push(&ProcessList, &Progress1.ProcessList);
  DbList_Push(&ProcessList, &Progress2.ProcessList);
  DbList_Push(&ProcessListAppend, &Progress3.ProcessList);
  DbList_Splice(&ProcessList, &ProcessListAppend);
  LONGS_EQUAL(3, DbList_Len(&ProcessList));
}
