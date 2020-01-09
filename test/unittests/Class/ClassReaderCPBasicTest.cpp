
#include "JVM/Class/ClassFileReader.h"
#include "gtest/gtest.h"
#include <fstream>
#include <unistd.h>

#include "ClassReaderTest.h"
class ClassReader : public ClassReaderBase {
  const char *getFilename() const override { return "Basic.class"; }
};

using namespace Class;
using namespace std::string_literals;

// Tests that the reader fails with a sensible warning when the class file
// magic int is not as expected.
TEST_F(ClassReader, IncorrectMagic) {
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

constexpr int majorVersion =
#include "Test_MajorVersion.inc"
    ;

constexpr int minorVersion =
#include "Test_MinorVersion.inc"
    ;

TEST_F(ClassReader, ParseVersion) {
  EXPECT_EQ(classFile->getMajorVersion(), majorVersion);
  EXPECT_EQ(classFile->getMinorVersion(), minorVersion);
}

constexpr int stringLiteralIndex =
#include "Test_StringLiteralIndex.inc"
    ;

constexpr int stringNameTypeIndex =
#include "Test_StringNameTypeIndex.inc"
    ;

TEST_F(ClassReader, ConstTableString) {
  auto &constPool = classFile->getConstPool();

  auto &utf8 = constPool.get<ConstPool::Utf8Info>(stringLiteralIndex);
  EXPECT_EQ("string literal"s, std::string(utf8));

  auto &nameType =
      constPool.get<ConstPool::NameAndTypeInfo>(stringNameTypeIndex);
  auto &utf82 = constPool.get<ConstPool::Utf8Info>(nameType.nameIndex);
  EXPECT_EQ("testString"s, std::string(utf82));

  auto utf83 = constPool.get<ConstPool::Utf8Info>(nameType.descriptorIndex);
  EXPECT_EQ("Ljava/lang/String;"s, std::string(utf83));
}

constexpr int intLiteralIndex =
#include "Test_IntegerLiteralIndex.inc"
    ;

constexpr int intNameTypeIndex =
#include "Test_IntegerNameTypeIndex.inc"
    ;

// Tests that the int in the const table is of the same value as expected in
// Basic.java
TEST_F(ClassReader, ConstTableInteger) {
  auto &constPool = classFile->getConstPool();

  auto &integer = constPool.get<ConstPool::IntegerInfo>(intLiteralIndex);
  EXPECT_EQ(integer.bytes, 32768);
  static_assert(std::is_same<decltype(integer.bytes), int32_t>::value);

  auto &nameType = constPool.get<ConstPool::NameAndTypeInfo>(intNameTypeIndex);
  auto &utf8 = constPool.get<ConstPool::Utf8Info>(nameType.nameIndex);
  EXPECT_EQ("testInt"s, std::string(utf8));

  auto &utf82 = constPool.get<ConstPool::Utf8Info>(nameType.descriptorIndex);
  EXPECT_EQ("I"s, std::string(utf82));
}

constexpr int longLiteralIndex =
#include "Test_LongLiteralIndex.inc"
    ;

constexpr int longNameTypeIndex =
#include "Test_LongNameTypeIndex.inc"
    ;

TEST_F(ClassReader, ConstTableLong) {
  auto &constPool = classFile->getConstPool();

  auto &longLit = constPool.get<ConstPool::LongInfo>(longLiteralIndex);
  static_assert(std::is_same<decltype(longLit.bytes), int64_t>::value);
  EXPECT_EQ(longLit.bytes, 2);

  auto &nameType = constPool.get<ConstPool::NameAndTypeInfo>(longNameTypeIndex);
  auto &utf8 = constPool.get<ConstPool::Utf8Info>(nameType.nameIndex);
  EXPECT_EQ("testLong"s, std::string(utf8));

  auto &utf82 = constPool.get<ConstPool::Utf8Info>(nameType.descriptorIndex);
  EXPECT_EQ("J"s, std::string(utf82));
}

constexpr int doubleLiteralIndex =
#include "Test_DoubleLiteralIndex.inc"
    ;

TEST_F(ClassReader, ConstTableDouble) {
  auto &constPool = classFile->getConstPool();

  auto &doub = constPool.get<ConstPool::DoubleInfo>(doubleLiteralIndex);
  static_assert(std::is_same<decltype(doub.bytes), decimal64>::value);
  EXPECT_EQ(doub.bytes, 2.0);
}

constexpr int floatLiteralIndex =
#include "Test_FloatLiteralIndex.inc"
    ;

TEST_F(ClassReader, ConstTableFloat) {
  auto &constPool = classFile->getConstPool();

  auto &flo = constPool.get<ConstPool::FloatInfo>(floatLiteralIndex);
  static_assert(std::is_same<decltype(flo.bytes), decimal32>::value);
  EXPECT_EQ(flo.bytes, 4.0);
}

constexpr int intFieldRef =
#include "Test_IntFieldRefIndex.inc"
    ;

TEST_F(ClassReader, ConstTableFieldRef) {
  auto &constPool = classFile->getConstPool();

  auto &fieldRef = constPool.get<ConstPool::FieldrefInfo>(intFieldRef);
  auto &classInfo = constPool.get<ConstPool::ClassInfo>(fieldRef.classIndex);
  auto &utf8 = constPool.get<ConstPool::Utf8Info>(classInfo.nameIndex);
  EXPECT_EQ("Basic"s, std::string(utf8));

  auto &nameType =
      constPool.get<ConstPool::NameAndTypeInfo>(fieldRef.nameAndTypeIndex);
  auto &utf82 = constPool.get<ConstPool::Utf8Info>(nameType.nameIndex);
  EXPECT_EQ("testInt"s, std::string(utf82));
}

constexpr int accessFlags =
#include "Test_AccessFlags.inc"
    ;

TEST_F(ClassReader, AccessFlags) {
  EXPECT_EQ(classFile->getAccessFlags(), accessFlags);
}

TEST_F(ClassReader, ThisClass) {
  auto &constPool = classFile->getConstPool();

  auto &classInfo =
      constPool.get<ConstPool::ClassInfo>(classFile->getThisClass());
  auto &utf8 = constPool.get<ConstPool::Utf8Info>(classInfo.nameIndex);
  EXPECT_EQ("Basic"s, std::string(utf8));
}

TEST_F(ClassReader, SuperClass) {
  auto &constPool = classFile->getConstPool();

  auto &classInfo =
      constPool.get<ConstPool::ClassInfo>(classFile->getSuperClass());
  auto &utf8 = constPool.get<ConstPool::Utf8Info>(classInfo.nameIndex);
  EXPECT_EQ("java/lang/Object"s, std::string(utf8));
}
