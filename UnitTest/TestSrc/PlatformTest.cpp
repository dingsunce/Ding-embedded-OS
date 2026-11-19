#include <gtest/gtest.h>

#include "TestReset.h"

class Platform : public ::testing::Test
{
protected:
  void TearDown() override
  {
    TestReset();
  }
};

TEST_F(Platform, U8Test)
{
  u8 value = 0xff;
  value = value + 2;
  EXPECT_EQ(1, value);
}

TEST_F(Platform, S8ToU8Test)
{
  s8 value = -3;
  u8 value1 = (u8)value;
  EXPECT_EQ(253, value1);
  EXPECT_EQ(static_cast<u8>(-3), static_cast<u8>(253));
}

TEST_F(Platform, U8ToS8Test)
{
  u8 value = 131; // 0x83
  s8 value1 = (s8)value;
  EXPECT_EQ(-125, value1);
  EXPECT_EQ(static_cast<u8>(131), static_cast<u8>(value1));

  value = (u8)value1;
  EXPECT_EQ(131, value);
}

TEST_F(Platform, U16Test)
{
  u16 value = 0xffff;
  value = value + 2;
  EXPECT_EQ(1, value);
}

TEST_F(Platform, U32Test)
{
  u32 value = 0xffffffff;
  value = value + 2;
  EXPECT_EQ(1, value);
}

TEST_F(Platform, StringTest1)
{
  char string[] = "TEST";
  EXPECT_EQ(5, sizeof(string));
  EXPECT_EQ(4, strlen(string));
}

TEST_F(Platform, StringTest2)
{
  const char *string = "TESTTEST";
  // EXPECT_EQ(4, sizeof(string));  // 32bits architecture
  // EXPECT_EQ(8, sizeof(string));  // 64bits architecture
  EXPECT_EQ(1, sizeof(*string));
  EXPECT_EQ(8, strlen(string));
}

TEST_F(Platform, sprintfTest)
{
  char        buffer[10];
  char        expectResult[3] = {'1', '0', '\0'};
  const char *expectResultPoint = "10";

  sprintf(buffer, "%d", 10);
  EXPECT_STREQ("10", buffer);
  EXPECT_STREQ(expectResult, buffer);
  EXPECT_STREQ(expectResultPoint, buffer);
}

TEST_F(Platform, memsetTest)
{
  char buffer[10];
  char expectResult[3] = {'\0', '\0', '\0'};
  strcpy_s(buffer, "");

  EXPECT_EQ(0, strlen(buffer));
  EXPECT_EQ(0, buffer[0]);
  EXPECT_STREQ(expectResult, buffer);
}

TEST_F(Platform, zeroTest)
{
  EXPECT_EQ(0, '\0');
  EXPECT_EQ(sizeof('\0'), sizeof(u8));
}

TEST_F(Platform, ValidBitTest)
{
  u16 value = 0x7fff;
  EXPECT_TRUE(CHECK_BIT(value, 0));
  EXPECT_TRUE(CHECK_BIT(value, 14));
  EXPECT_FALSE(CHECK_BIT(value, 15));
}

TEST_F(Platform, SetBitTest)
{
  u16 value = 0;
  PLACE_BIT(value, 0);
  EXPECT_EQ(0x01, value);

  PLACE_BIT(value, 15);
  EXPECT_EQ(0x8001, value);
}

TEST_F(Platform, ClrBitTest)
{
  u16 value = 0x8001;
  CLEAN_BIT(value, 15);
  EXPECT_EQ(0x01, value);

  CLEAN_BIT(value, 0);
  EXPECT_EQ(0x00, value);
}

TEST_F(Platform, InvertBitTest)
{
  u16 value = 0x8001;
  CLEAN_BIT(value, 15);
  EXPECT_EQ(0x01, value);

  INVERT_BIT(value, 15);
  EXPECT_EQ(0x8001, value);

  value = 0x03;
  CLEAN_BIT(value, 1);
  EXPECT_EQ(0x01, value);

  INVERT_BIT(value, 1);
  EXPECT_EQ(0x03, value);
}

TEST_F(Platform, DivideTest)
{
  EXPECT_EQ(12, 100 / 8);
  EXPECT_EQ(6, 100 / 16);
  EXPECT_EQ(3, 100 / 32);
  EXPECT_EQ(1, 100 / 64);
}

TEST_F(Platform, MultifyTest)
{
  EXPECT_EQ(0, (u8)((float)2 * (float)(0.4)));
  EXPECT_EQ(1, (u8)((float)3 * (float)(0.4)));
  EXPECT_EQ(100, (u8)((float)252 * (float)(0.4)));
  EXPECT_EQ(101, (u8)((float)253 * (float)(0.4)));
}

TEST_F(Platform, U16ToU8PointorTest)
{
  u16 value = 0x1001;
  u8 *p = (u8 *)&value;
  EXPECT_EQ(*p, LO_U16(value));
  EXPECT_EQ(*(p + 1), HI_U16(value));
}

TEST_F(Platform, U8ToU16PointorTest)
{
  u8 value[2] = {LO_HI_U16(0x55aa)};
  EXPECT_EQ(0x55aa, (*(u16 *)value));
}

TEST_F(Platform, LittleEndJudgment)
{
  u32 x = 1;
  EXPECT_EQ(1, *(u8 *)&x);
}

TEST_F(Platform, FloatToU8)
{
  float x = (float)1.65;
  EXPECT_EQ(1, (u8)x);
}

TEST_F(Platform, FloatToU8_UpperRound)
{
  float x = (float)1.50;
  x = (float)(x * (float)(10) + (float)(5)) / (float)(10);
  EXPECT_EQ(2, (u8)x);
}

TEST_F(Platform, FloatToU8_DownRound)
{
  float x = (float)1.49;
  x = (float)(x * (float)(10) + (float)(5)) / (float)(10);
  EXPECT_EQ(1, (u8)x);
}

TEST_F(Platform, NegtiveNumCompare)
{
  s8 test = -2;
  EXPECT_EQ(254, (u8)test);
}

TEST_F(Platform, Mask)
{
  u32 mask = -1;
  EXPECT_EQ(0xffffffff, mask);
  mask += mask;
  EXPECT_EQ(0xfffffffe, mask);
  mask += mask;
  EXPECT_EQ(0xfffffffc, mask);
  mask += mask;
  EXPECT_EQ(0xfffffff8, mask);
}

TEST_F(Platform, negative)
{
  u32 mask = -1;
  EXPECT_EQ(0xffffffff, mask);

  mask = -2;
  EXPECT_EQ(0xfffffffe, mask);
}
