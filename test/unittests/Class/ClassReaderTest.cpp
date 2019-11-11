
#include "JVM/Class/ClassFileReader.h"
#include "gtest/gtest.h"
#include <fstream>
#include <unistd.h>

constexpr int majorVersion =
#include "Test_MajorVersion.inc"
    ;

constexpr int minorVersion =
#include "Test_MinorVersion.inc"
    ;

constexpr int stringLiteralIndex =
#include "Test_StringLiteralIndex.inc"
    ;

constexpr int stringNameTypeIndex =
#include "Test_StringNameTypeIndex.inc"
    ;

constexpr int intLiteralIndex =
#include "Test_IntegerLiteralIndex.inc"
    ;

constexpr int intNameTypeIndex =
#include "Test_IntegerNameTypeIndex.inc"
    ;

constexpr int longLiteralIndex =
#include "Test_LongLiteralIndex.inc"
    ;

constexpr int longNameTypeIndex =
#include "Test_LongNameTypeIndex.inc"
    ;

using namespace Class;

TEST(ClassReader, FindFiles) { ASSERT_NE(access("Test.class", 0), -1); }

TEST(ClassReader, IncorrectMagic) {
  std::ofstream s("incorrect");
  uint64_t notMagic = 0xCAFEBABF;
  s.write(reinterpret_cast<const char *>(&notMagic), 8);
  s.close();

  {
    ClassFileReader reader("incorrect");
    auto fileOrError = reader.read();
    EXPECT_EQ(fileOrError.second, "incorrect file format");
  }

  s.open("incorrect", std::ios_base::trunc);
  uint8_t arr[] = {0xCA, 0xFE, 0xBA, 0xBE};
  s.write(reinterpret_cast<const char *>(arr), 1);
  s.write(reinterpret_cast<const char *>(arr + 1), 1);
  s.write(reinterpret_cast<const char *>(arr + 2), 1);
  s.write(reinterpret_cast<const char *>(arr + 3), 1);
  s.close();

  {
    ClassFileReader reader("incorrect");
    auto fileOrError = reader.read();
    EXPECT_EQ(fileOrError.second, "reader error");
  }
}

TEST(ClassReader, ParseVersion) {
  ClassFileReader reader("Test.class");
  auto fileOrError = reader.read();
  ASSERT_EQ(fileOrError.second, std::string());
  std::unique_ptr<ClassFile> classFile = std::move(fileOrError.first);
  EXPECT_EQ(classFile->getMajorVersion(), majorVersion);
  EXPECT_EQ(classFile->getMinorVersion(), minorVersion);
}

TEST(ClassReader, ConstTableString) {
  ClassFileReader reader("Test.class");
  auto fileOrError = reader.read();
  ASSERT_EQ(fileOrError.second, std::string());
  std::unique_ptr<ClassFile> classFile = std::move(fileOrError.first);
  auto &entries = classFile->getConstPool().getEntries();

  ASSERT_GT(entries.size(), stringLiteralIndex + 1);
  ASSERT_EQ(entries[stringLiteralIndex]->tag, ConstPool::Utf8);
  auto &utf8 = *reinterpret_cast<const ConstPool::Utf8Info *>(
      entries[stringLiteralIndex].get());
  ASSERT_EQ(strlen("string literal"), utf8.length);
  int diff = strncmp(reinterpret_cast<const char *>(utf8.bytes),
                     "string literal", utf8.length);
  EXPECT_FALSE(diff);

  ASSERT_EQ(entries[stringNameTypeIndex]->tag, ConstPool::NameAndType);
  auto &nameType = *reinterpret_cast<const ConstPool::NameAndTypeInfo *>(
      entries[stringNameTypeIndex].get());
  ASSERT_EQ(entries[nameType.nameIndex]->tag, ConstPool::Utf8);
  auto &utf82 = *reinterpret_cast<const ConstPool::Utf8Info *>(
      entries[nameType.nameIndex].get());
  ASSERT_EQ(strlen("testString"), utf82.length);
  diff = strncmp(reinterpret_cast<const char *>(utf82.bytes), "testString",
                 utf82.length);
  EXPECT_FALSE(diff);
  ASSERT_EQ(entries[nameType.descriptorIndex]->tag, ConstPool::Utf8);
  auto &utf83 = *reinterpret_cast<const ConstPool::Utf8Info *>(
      entries[nameType.descriptorIndex].get());
  ASSERT_EQ(strlen("Ljava/lang/String;"), utf83.length);
  diff = strncmp(reinterpret_cast<const char *>(utf83.bytes),
                 "Ljava/lang/String;", utf83.length);
  EXPECT_FALSE(diff);
}

TEST(ClassReader, ConstTableInteger) {
  ClassFileReader reader("Test.class");
  auto fileOrError = reader.read();
  ASSERT_EQ(fileOrError.second, std::string());
  std::unique_ptr<ClassFile> classFile = std::move(fileOrError.first);
  auto &entries = classFile->getConstPool().getEntries();

  ASSERT_GT(entries.size(), intLiteralIndex);
  ASSERT_EQ(entries[intLiteralIndex]->tag, ConstPool::Integer);
  auto &integer = *reinterpret_cast<const ConstPool::IntegerInfo *>(
      entries[intLiteralIndex].get());
  EXPECT_EQ(integer.bytes, 32768);

  ASSERT_GT(entries.size(), intNameTypeIndex);
  ASSERT_EQ(entries[intNameTypeIndex]->tag, ConstPool::NameAndType);
  auto &nameType = *reinterpret_cast<const ConstPool::NameAndTypeInfo *>(
      entries[intNameTypeIndex].get());
  int nameIndex = nameType.nameIndex;
  ASSERT_GT(entries.size(), nameIndex);
  ASSERT_EQ(entries[nameIndex]->tag, ConstPool::Utf8);
  auto &utf8 =
      *reinterpret_cast<const ConstPool::Utf8Info *>(entries[nameIndex].get());
  EXPECT_EQ(strlen("testInt"), utf8.length);
  int diff = strncmp(reinterpret_cast<const char *>(utf8.bytes), "testInt",
                     utf8.length);
  EXPECT_FALSE(diff);

  int typeIndex = nameType.descriptorIndex;
  ASSERT_GT(entries.size(), typeIndex);
  ASSERT_EQ(entries[typeIndex]->tag, ConstPool::Utf8);
  auto &utf82 =
      *reinterpret_cast<const ConstPool::Utf8Info *>(entries[typeIndex].get());
  EXPECT_EQ(utf82.length, 1);
  EXPECT_EQ(utf82.bytes[0], 'I');
}

TEST(ClassReader, ConstTableLong) {
  ClassFileReader reader("Test.class");
  auto fileOrError = reader.read();
  ASSERT_EQ(fileOrError.second, std::string());
  std::unique_ptr<ClassFile> classFile = std::move(fileOrError.first);
  auto &entries = classFile->getConstPool().getEntries();

  ASSERT_GT(entries.size(), longLiteralIndex);
  ASSERT_EQ(entries[longLiteralIndex]->tag, ConstPool::Long);
  EXPECT_EQ(entries[longLiteralIndex + 1], nullptr);
  auto &longLit = *reinterpret_cast<const ConstPool::LongInfo *>(
      entries[longLiteralIndex].get());
  EXPECT_EQ(longLit.bytes, 2);

  ASSERT_GT(entries.size(), longNameTypeIndex);
  ASSERT_EQ(entries[longNameTypeIndex]->tag, ConstPool::NameAndType);
  auto &nameType = *reinterpret_cast<const ConstPool::NameAndTypeInfo *>(
      entries[longNameTypeIndex].get());
  int nameIndex = nameType.nameIndex;
  ASSERT_EQ(entries[nameIndex]->tag, ConstPool::Utf8);
  auto &utf8 =
      *reinterpret_cast<const ConstPool::Utf8Info *>(entries[nameIndex].get());
  ASSERT_EQ(strlen("testLong"), utf8.length);
  EXPECT_FALSE(strncmp(reinterpret_cast<const char *>(utf8.bytes), "testLong",
                       utf8.length));

  int typeIndex = nameType.descriptorIndex;
  ASSERT_GT(entries.size(), typeIndex);
  ASSERT_EQ(entries[typeIndex]->tag, ConstPool::Utf8);
  auto &utf82 =
      *reinterpret_cast<const ConstPool::Utf8Info *>(entries[typeIndex].get());
  EXPECT_EQ(utf82.length, 1);
  EXPECT_EQ(utf82.bytes[0], 'J');
}
