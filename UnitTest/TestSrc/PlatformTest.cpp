#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport_c.h"

#include "TestReset.h"

TEST_GROUP(Platform){TEST_SETUP(){TestOutput::enableCompactFormat();
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

TEST(Platform, U8Test)
{
    u8 value = 0xff;
    value = value + 2;
    LONGS_EQUAL(1, value);
}

TEST(Platform, S8ToU8Test)
{
    s8 value = -3;
    u8 value1 = (u8)value;
    LONGS_EQUAL(0xfd, value1);
    BYTES_EQUAL(-3, 0xfd);
    LONGS_EQUAL(-3, 0xffffffffd);
}

TEST(Platform, U8ToS8Test)
{
    u8 value = 0x83;
    s8 value1 = (s8)value;
    LONGS_EQUAL(0xffffff83, value1);
    BYTES_EQUAL(0x83, value1);

    value = (u8)value1;
    LONGS_EQUAL(0x83, value);
}

TEST(Platform, U16Test)
{
    u16 value = 0xffff;
    value = value + 2;
    LONGS_EQUAL(1, value);
}

TEST(Platform, U32Test)
{
    u32 value = 0xffffffff;
    value = value + 2;
    LONGS_EQUAL(1, value);
}

TEST(Platform, StringTest1)
{
    char string[] = "TEST";
    LONGS_EQUAL(5, sizeof(string));
    LONGS_EQUAL(4, strlen(string));
}

TEST(Platform, StringTest2)
{
    const char *string = "TESTTEST";
    // LONGS_EQUAL(4, sizeof(string));  // 32bits architecture
    // LONGS_EQUAL(8, sizeof(string));  // 64bits architecture
    LONGS_EQUAL(1, sizeof(*string));
    LONGS_EQUAL(8, strlen(string));
}

TEST(Platform, sprintfTest)
{
    char        buffer[10];
    char        expectResult[3] = {'1', '0', '\0'};
    const char *expectResultPoint = "10";

    sprintf(buffer, "%d", 10);
    STRCMP_EQUAL("10", buffer);
    STRCMP_EQUAL(expectResult, buffer);
    STRCMP_EQUAL(expectResultPoint, buffer);
}

TEST(Platform, memsetTest)
{
    char buffer[10];
    char expectResult[3] = {'\0', '\0', '\0'};
    strcpy(buffer, "");

    LONGS_EQUAL(0, strlen(buffer));
    LONGS_EQUAL(0, buffer[0]);
    STRCMP_EQUAL(expectResult, buffer);
}

TEST(Platform, zeroTest)
{
    LONGS_EQUAL(0, '\0');
    LONGS_EQUAL(sizeof('\0'), sizeof(u8));
}

TEST(Platform, ValidBitTest)
{
    u16 value = 0x7fff;
    CHECK_TRUE(CHECK_BIT(value, 0));
    CHECK_TRUE(CHECK_BIT(value, 14));
    CHECK_FALSE(CHECK_BIT(value, 15));
}

TEST(Platform, SetBitTest)
{
    u16 value = 0;
    PLACE_BIT(value, 0);
    LONGS_EQUAL(0x01, value);

    PLACE_BIT(value, 15);
    LONGS_EQUAL(0x8001, value);
}

TEST(Platform, ClrBitTest)
{
    u16 value = 0x8001;
    CLEAN_BIT(value, 15);
    LONGS_EQUAL(0x01, value);

    CLEAN_BIT(value, 0);
    LONGS_EQUAL(0x00, value);
}

TEST(Platform, DivideTest)
{
    LONGS_EQUAL(12, 100 / 8);
    LONGS_EQUAL(6, 100 / 16);
    LONGS_EQUAL(3, 100 / 32);
    LONGS_EQUAL(1, 100 / 64);
}

TEST(Platform, MultifyTest)
{
    LONGS_EQUAL(0, (u8)((float)2 * (float)(0.4)));
    LONGS_EQUAL(1, (u8)((float)3 * (float)(0.4)));
    LONGS_EQUAL(100, (u8)((float)252 * (float)(0.4)));
    LONGS_EQUAL(101, (u8)((float)253 * (float)(0.4)));
}

TEST(Platform, U16ToU8PointorTest)
{
    u16 value = 0x1001;
    u8 *p = (u8 *)&value;
    LONGS_EQUAL(*p, LOBYTE(value));
    LONGS_EQUAL(*(p + 1), HIBYTE(value));
}

TEST(Platform, U8ToU16PointorTest)
{
    u8 value[2] = {LOHI(0x55aa)};
    LONGS_EQUAL(0x55aa, (*(u16 *)value));
}

TEST(Platform, LittleEndJudgment)
{
    u32 x = 1;
    LONGS_EQUAL(1, *(u8 *)&x);
}

TEST(Platform, FloatToU8)
{
    float x = (float)1.65;
    LONGS_EQUAL(1, (u8)x);
}

TEST(Platform, FloatToU8_UpperRound)
{
    float x = (float)1.50;
    x = (float)(x * (float)(10) + (float)(5)) / (float)(10);
    LONGS_EQUAL(2, (u8)x);
}

TEST(Platform, FloatToU8_DownRound)
{
    float x = (float)1.49;
    x = (float)(x * (float)(10) + (float)(5)) / (float)(10);
    LONGS_EQUAL(1, (u8)x);
}

TEST(Platform, NegtiveNumCompare)
{
    s8 test = -2;
    LONGS_EQUAL(254, (u8)test);
}

TEST(Platform, Mask)
{
    u32 mask = -1;
    LONGS_EQUAL(0xffffffff, mask);
    mask += mask;
    LONGS_EQUAL(0xfffffffe, mask);
    mask += mask;
    LONGS_EQUAL(0xfffffffc, mask);
    mask += mask;
    LONGS_EQUAL(0xfffffff8, mask);
}

TEST(Platform, negative)
{
    u32 mask = -1;
    LONGS_EQUAL(0xffffffff, mask);

    mask = -2;
    LONGS_EQUAL(0xfffffffe, mask);
}
