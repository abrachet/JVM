
#include "JVM/VM/TypeReader.h"
#include "gtest/gtest.h"

TEST(TypeReader, Basic) {
  ErrorOr<FuncOrSingleType> type = parseType("I");
  ASSERT_TRUE(type);
  EXPECT_EQ(type->second.size(), 0);
  EXPECT_EQ(type->first.first, Int);
  type = parseType("J");
  ASSERT_TRUE(type);
  EXPECT_EQ(type->second.size(), 0);
  EXPECT_EQ(type->first.first, Long);
  type = parseType("B");
  ASSERT_TRUE(type);
  EXPECT_EQ(type->second.size(), 0);
  EXPECT_EQ(type->first.first, Byte);
  type = parseType("S");
  ASSERT_TRUE(type);
  EXPECT_EQ(type->second.size(), 0);
  EXPECT_EQ(type->first.first, Short);
  type = parseType("C");
  ASSERT_TRUE(type);
  EXPECT_EQ(type->second.size(), 0);
  EXPECT_EQ(type->first.first, Char);
  type = parseType("F");
  ASSERT_TRUE(type);
  EXPECT_EQ(type->second.size(), 0);
  EXPECT_EQ(type->first.first, Float);
  type = parseType("D");
  ASSERT_TRUE(type);
  EXPECT_EQ(type->second.size(), 0);
  EXPECT_EQ(type->first.first, Double);
}

TEST(TypeReader, Object) {
  ErrorOr<FuncOrSingleType> type = parseType("Ljava/lang/Object;");
  ASSERT_TRUE(type);
  EXPECT_EQ(type->second.size(), 0);
  EXPECT_EQ(type->first.first, Object);
  EXPECT_EQ(type->first.second, "java/lang/Object");
}

TEST(TypeReader, BasicFunction) {
  ErrorOr<FuncOrSingleType> type = parseType("(I)J");
  ASSERT_TRUE(type);
  EXPECT_EQ(type->second.size(), 2);
  EXPECT_EQ(type->first.first, Function);
  EXPECT_EQ(type->second[0].first, Long);
  EXPECT_EQ(type->second[1].first, Int);
}

TEST(TypeReader, ComplexFunction) {
  ErrorOr<FuncOrSingleType> type =
      parseType("(Ljava/lang/Object;Ljava/lang/String;C)J");
  ASSERT_TRUE(type);
  EXPECT_EQ(type->second.size(), 4);
  EXPECT_EQ(type->first.first, Function);
  EXPECT_EQ(type->second[0].first, Long);
  EXPECT_EQ(type->second[1].first, Object);
  EXPECT_EQ(type->second[1].second, "java/lang/Object");
  EXPECT_EQ(type->second[2].first, Object);
  EXPECT_EQ(type->second[2].second, "java/lang/String");
  EXPECT_EQ(type->second[3].first, Char);
}
