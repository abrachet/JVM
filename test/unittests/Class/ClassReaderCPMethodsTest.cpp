
#include "JVM/Class/ClassFileReader.h"
#include "gtest/gtest.h"
#include <fstream>
#include <unistd.h>

#include "ClassReaderTest.h"

class ClassReaderMethod : public ClassReaderBase {
  const char *getFilename() const override { return "InvD.class"; }
};

using namespace Class;

constexpr int invDynIndex =
#include "InvD_InvokeDynamicIndex.inc"
    ;

constexpr int invDynBootstrapMethod =
#include "InvD_InvDBSMethod.inc"
    ;

constexpr int invDynNameType =
#include "InvD_InvDNameTypeIndex.inc"
    ;

TEST_F(ClassReaderMethod, InvokeDynamic) {
  auto &constPool = classFile->getConstPool();

  auto &invD = constPool.get<ConstPool::InvokeDynamicInfo>(invDynIndex);
  EXPECT_EQ(invD.bootstrapMethodAttrIndex, invDynBootstrapMethod);
  EXPECT_EQ(invD.nameAndTypeIndex, invDynNameType);
}

constexpr int methodRefIndex =
#include "InvD_FirstMethodRefIndex.inc"
    ;

TEST_F(ClassReaderMethod, MethodRef) {
  auto &constPool = classFile->getConstPool();
  auto &entries = constPool.getEntries();

  auto &methodRef = constPool.get<ConstPool::MethodrefInfo>(methodRefIndex);

  int classIndex = methodRef.classIndex;
  ASSERT_GT(entries.size(), classIndex);
  ASSERT_EQ(entries[classIndex]->tag, ConstPool::Class);

  int nameTypeIndex = methodRef.nameAndTypeIndex;
  ASSERT_GT(entries.size(), nameTypeIndex);
  ASSERT_EQ(entries[nameTypeIndex]->tag, ConstPool::NameAndType);
}

constexpr int interfaceMethodIndex =
#include "InvD_FirstInterfaceMethodrefIndex.inc"
    ;

TEST_F(ClassReaderMethod, InterfaceMethodRef) {
  auto &constPool = classFile->getConstPool();
  auto &entries = constPool.getEntries();

  auto &methodRef =
      constPool.get<ConstPool::InterfaceMethodrefInfo>(interfaceMethodIndex);

  int classIndex = methodRef.classIndex;
  ASSERT_GT(entries.size(), classIndex);
  ASSERT_EQ(entries[classIndex]->tag, ConstPool::Class);

  int nameTypeIndex = methodRef.nameAndTypeIndex;
  ASSERT_GT(entries.size(), nameTypeIndex);
  ASSERT_EQ(entries[nameTypeIndex]->tag, ConstPool::NameAndType);
}

constexpr int methodHandleIndex =
#include "InvD_FirstMethodHandleIndex.inc"
    ;

TEST_F(ClassReaderMethod, MethodHandle) {
  auto &constPool = classFile->getConstPool();
  auto &entries = constPool.getEntries();

  auto &methodHandle =
      constPool.get<ConstPool::MethodHandleInfo>(methodHandleIndex);

  EXPECT_TRUE(methodHandle.referenceKind > 0);
  EXPECT_TRUE(methodHandle.referenceKind < 10);

  int methodRefIndex = methodHandle.referenceIndex;
  ASSERT_GT(entries.size(), methodRefIndex);
  ASSERT_EQ(entries[methodRefIndex]->tag, ConstPool::Methodref);
}

constexpr int methodTypeIndex =
#include "InvD_FirstMethodTypeIndex.inc"
    ;

TEST_F(ClassReaderMethod, MethodType) {
  auto &constPool = classFile->getConstPool();
  auto &entries = constPool.getEntries();

  auto &methodType = constPool.get<ConstPool::MethodTypeInfo>(methodTypeIndex);

  int descriptorIndex = methodType.descriptorIndex;
  ASSERT_GT(entries.size(), descriptorIndex);
  ASSERT_EQ(entries[descriptorIndex]->tag, ConstPool::Utf8);
}
