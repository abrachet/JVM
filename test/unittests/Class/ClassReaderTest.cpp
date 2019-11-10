
#include <unistd.h>
#include <fstream>
#include "JVM/Class/ClassFileReader.h"
#include "gtest/gtest.h"

constexpr int majorVersion =
#include "Test_MajorVersion.inc"
    ;

constexpr int minorVersion =
#include "Test_MinorVersion.inc"
    ;

TEST(ClassReader, FindFiles) {
  ASSERT_NE(access("Test.class", 0), -1);
}

TEST(ClassReader, IncorrectMagic) {
  std::ofstream s("incorrect");
  uint64_t notMagic = 0xCAFEBABF;
  s.write(reinterpret_cast<const char*>(&notMagic), 8);
  s.close();

  {
    ClassFileReader reader("incorrect");
    auto fileOrError = reader.read();
    EXPECT_EQ(fileOrError.second, "incorrect file format");
  }

  s.open("incorrect", std::ios_base::trunc);
  uint8_t arr[] = {0xCA, 0xFE, 0xBA, 0xBE};
  s.write(reinterpret_cast<const char*>(arr), 1);
  s.write(reinterpret_cast<const char*>(arr + 1), 1);
  s.write(reinterpret_cast<const char*>(arr + 2), 1);
  s.write(reinterpret_cast<const char*>(arr + 3), 1);
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
