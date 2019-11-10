
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

TEST(ClassReader, ParseConstTable) {
  ClassFileReader reader("Test.class");
  auto fileOrError = reader.read();
  ASSERT_EQ(fileOrError.second, std::string());
  std::unique_ptr<ClassFile> classFile = std::move(fileOrError.first);
  auto &entries = classFile->getConstPool().getEntries();
  ASSERT_GT(entries.size(), stringLiteralIndex + 1);
  ASSERT_EQ(entries[stringLiteralIndex]->tag, ConstPool::Utf8);
  auto &utf8 = *reinterpret_cast<const ConstPool::Utf8Info *>(
      entries[stringLiteralIndex].get());
  int diff =
      strncmp(reinterpret_cast<const char *>(utf8.bytes), "string literal", 14);
  EXPECT_FALSE(diff);

  ASSERT_EQ(entries[stringLiteralIndex + 1]->tag, ConstPool::NameAndType);
  auto &nameType = *reinterpret_cast<const ConstPool::NameAndTypeInfo *>(
      entries[stringLiteralIndex + 1].get());
  ASSERT_EQ(entries[nameType.nameIndex]->tag, ConstPool::Utf8);
  auto &utf82 = *reinterpret_cast<const ConstPool::Utf8Info *>(
      entries[nameType.nameIndex].get());
  diff = strncmp(reinterpret_cast<const char *>(utf82.bytes), "testString", 10);
  EXPECT_FALSE(diff);
  ASSERT_EQ(entries[nameType.descriptorIndex]->tag, ConstPool::Utf8);
  auto &utf83 = *reinterpret_cast<const ConstPool::Utf8Info *>(
      entries[nameType.descriptorIndex].get());
  diff = strncmp(reinterpret_cast<const char *>(utf83.bytes),
                 "Ljava/lang/String;", 18);
  EXPECT_FALSE(diff);
}
