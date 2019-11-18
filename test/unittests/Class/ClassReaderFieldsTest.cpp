
#include "JVM/Class/ClassFileReader.h"
#include "gtest/gtest.h"
#include <cstring>

#include "ClassReaderTest.h"

using namespace Class;

class ClassReaderFields : public ClassReaderBase {
  const char *getFilename() const override { return "Basic.class"; }
};

TEST_F(ClassReaderFields, NumFields) {
  EXPECT_EQ(classFile->getFields().size(), 6);
}

TEST_F(ClassReaderFields, AccessFlags) {
  const auto &fields = classFile->getFields();
  EXPECT_EQ(fields[0].accessFlags, Field::Public | Field::Static);
  EXPECT_EQ(fields[1].accessFlags, Field::Private | Field::Static);
  EXPECT_EQ(fields[2].accessFlags, Field::Protected | Field::Static);
  EXPECT_EQ(fields[3].accessFlags, Field::Volatile | Field::Static);
  EXPECT_EQ(fields[4].accessFlags, Field::Transient | Field::Static);
  EXPECT_EQ(fields[5].accessFlags,
            Field::Public | Field::Volatile | Field::Transient);
}

TEST_F(ClassReaderFields, NameIndex) {
  const auto &fields = classFile->getFields();
  const auto &entries = classFile->getConstPool().getEntries();
  const Field &testString = fields[0];
  int nameIndex = testString.nameIndex;
  ASSERT_EQ(entries[nameIndex]->tag, ConstPool::Utf8);
  auto &utf8 =
      *reinterpret_cast<const ConstPool::Utf8Info *>(entries[nameIndex].get());
  EXPECT_EQ(strlen("testString"), utf8.length);
  EXPECT_FALSE(strncmp("testString", reinterpret_cast<const char *>(utf8.bytes),
                       utf8.length));
}

TEST_F(ClassReaderFields, Descriptor) {
  const auto &fields = classFile->getFields();
  const auto &entries = classFile->getConstPool().getEntries();
  const Field &testString = fields[0];
  int descriptorIndex = testString.descriptorIndex;
  ASSERT_EQ(entries[descriptorIndex]->tag, ConstPool::Utf8);
  auto &utf8 = *reinterpret_cast<const ConstPool::Utf8Info *>(
      entries[descriptorIndex].get());
  EXPECT_EQ(strlen("Ljava/lang/String;"), utf8.length);
  EXPECT_FALSE(strncmp("Ljava/lang/String;",
                       reinterpret_cast<const char *>(utf8.bytes),
                       utf8.length));

  const Field &testInt = fields[1];
  descriptorIndex = testInt.descriptorIndex;
  ASSERT_EQ(entries[descriptorIndex]->tag, ConstPool::Utf8);
  auto &utf82 = *reinterpret_cast<const ConstPool::Utf8Info *>(
      entries[descriptorIndex].get());
  EXPECT_EQ(strlen("I"), utf82.length);
  EXPECT_FALSE(
      strncmp("I", reinterpret_cast<const char *>(utf82.bytes), utf82.length));
}

// TODO: test attributes
