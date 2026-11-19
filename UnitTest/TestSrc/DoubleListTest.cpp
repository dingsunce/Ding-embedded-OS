#include <gtest/gtest.h>

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

class DList : public ::testing::Test
{
protected:
  void TearDown() override
  {
    DbList_Init(&ProcessList);
    TestReset();
  }
};

TEST_F(DList, Init)
{
  EXPECT_EQ(0, DbList_Len(&ProcessList));
  EXPECT_TRUE(DbList_IsEmpty(&ProcessList));
  EXPECT_EQ(nullptr, DbList_Pop(&ProcessList));
}

TEST_F(DList, PushPop)
{
  DbList_Push(&ProcessList, &Progress1.ProcessList);
  DbList_Push(&ProcessList, &Progress2.ProcessList);

  EXPECT_EQ(2, DbList_Len(&ProcessList));
  EXPECT_FALSE(DbList_IsEmpty(&ProcessList));

  Db_List_t *l1 = DbList_Pop(&ProcessList);
  Db_List_t *l2 = DbList_Pop(&ProcessList);
  EXPECT_EQ(&Progress2.ProcessList, l1);
  EXPECT_EQ(&Progress1.ProcessList, l2);
  EXPECT_EQ(&Progress2, DContainerOf(l1, DProcess_t, ProcessList));
  EXPECT_EQ(&Progress1, DContainerOf(l2, DProcess_t, ProcessList));
}

TEST_F(DList, AddPop)
{
  DbList_Add(&ProcessList, &Progress1.ProcessList);
  DbList_Add(&ProcessList, &Progress2.ProcessList);

  EXPECT_EQ(2, DbList_Len(&ProcessList));
  EXPECT_FALSE(DbList_IsEmpty(&ProcessList));

  Db_List_t *l1 = DbList_Pop(&ProcessList);
  Db_List_t *l2 = DbList_Pop(&ProcessList);
  EXPECT_EQ(&Progress1.ProcessList, l1);
  EXPECT_EQ(&Progress2.ProcessList, l2);
  EXPECT_EQ(&Progress1, DContainerOf(l1, DProcess_t, ProcessList));
  EXPECT_EQ(&Progress2, DContainerOf(l2, DProcess_t, ProcessList));
}

TEST_F(DList, Remove)
{
  DbList_Push(&ProcessList, &Progress1.ProcessList);
  DbList_Push(&ProcessList, &Progress2.ProcessList);
  DbList_Remove(&Progress1.ProcessList);
  DbList_Remove(&Progress2.ProcessList);

  EXPECT_EQ(0, DbList_Len(&ProcessList));
  EXPECT_TRUE(DbList_IsEmpty(&ProcessList));
  EXPECT_EQ(nullptr, DbList_Pop(&ProcessList));
}

TEST_F(DList, ReInit)
{
  DbList_Push(&ProcessList, &Progress1.ProcessList);
  DbList_Push(&ProcessList, &Progress2.ProcessList);
  DbList_Init(&ProcessList);

  EXPECT_EQ(0, DbList_Len(&ProcessList));
  EXPECT_TRUE(DbList_IsEmpty(&ProcessList));
  EXPECT_EQ(nullptr, DbList_Pop(&ProcessList));
}

TEST_F(DList, Spilce)
{
  DbList_Push(&ProcessList, &Progress1.ProcessList);
  DbList_Push(&ProcessList, &Progress2.ProcessList);
  DbList_Push(&ProcessListAppend, &Progress3.ProcessList);
  DbList_Splice(&ProcessList, &ProcessListAppend);
  EXPECT_EQ(3, DbList_Len(&ProcessList));
}
