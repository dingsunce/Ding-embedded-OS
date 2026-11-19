#include <gtest/gtest.h>
#include "TestReset.h"

class QuietListener : public ::testing::EmptyTestEventListener
{
    void OnTestPartResult(const ::testing::TestPartResult& result) override
    {
        if (result.failed())
        {
            printf("[FAIL] %s:%d: %s\n", result.file_name(), result.line_number(), result.summary());
        }
    }

    void OnTestProgramEnd(const ::testing::UnitTest& unit_test) override
    {
        int total = unit_test.total_test_count();
        int failed = unit_test.failed_test_count();
        if (failed == 0)
        {
            printf("[PASS] Ran %d test(s), all passed.\n", total);
        }
        else
        {
            printf("[FAIL] Ran %d test(s), %d failed.\n", total, failed);
        }
    }
};

int main(int argc, char** argv)
{
    TestReset();
    ::testing::InitGoogleTest(&argc, argv);

    auto& listeners = ::testing::UnitTest::GetInstance()->listeners();

    delete listeners.Release(listeners.default_result_printer());
    listeners.Append(new QuietListener);

    return RUN_ALL_TESTS();
}
