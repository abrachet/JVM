
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
  auto &entries = classFile->getConstPool().getEntries();

  ASSERT_GT(entries.size(), invDynIndex);
  ASSERT_EQ(entries[invDynIndex]->tag, ConstPool::InvokeDynamic);
  auto &invD = *reinterpret_cast<const ConstPool::InvokeDynamicInfo *>(
      entries[invDynIndex].get());

  EXPECT_EQ(invD.bootstrapMethodAttrIndex, invDynBootstrapMethod);
  EXPECT_EQ(invD.nameAndTypeIndex, invDynNameType);
}

constexpr int methodRefIndex =
#include "InvD_FirstMethodRefIndex.inc"
    ;

TEST_F(ClassReaderMethod, MethodRef) {
  auto &entries = classFile->getConstPool().getEntries();

  ASSERT_GT(entries.size(), methodRefIndex);
  ASSERT_EQ(entries[methodRefIndex]->tag, ConstPool::Methodref);
  auto &methodRef = *reinterpret_cast<const ConstPool::MethodrefInfo *>(
      entries[methodRefIndex].get());

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
  auto &entries = classFile->getConstPool().getEntries();

  ASSERT_GT(entries.size(), interfaceMethodIndex);
  ASSERT_EQ(entries[interfaceMethodIndex]->tag, ConstPool::InterfaceMethodref);
  auto &methodRef =
      *reinterpret_cast<const ConstPool::InterfaceMethodrefInfo *>(
          entries[interfaceMethodIndex].get());

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
  auto &entries = classFile->getConstPool().getEntries();

  ASSERT_GT(entries.size(), methodHandleIndex);
  ASSERT_EQ(entries[methodHandleIndex]->tag, ConstPool::MethodHandle);

  auto &methodHandle = *reinterpret_cast<const ConstPool::MethodHandleInfo *>(
      entries[methodHandleIndex].get());
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
  auto &entries = classFile->getConstPool().getEntries();

  ASSERT_GT(entries.size(), methodTypeIndex);
  ASSERT_EQ(entries[methodTypeIndex]->tag, ConstPool::MethodType);

  auto &methodType = *reinterpret_cast<const ConstPool::MethodTypeInfo *>(
      entries[methodTypeIndex].get());

  int descriptorIndex = methodType.descriptorIndex;
  ASSERT_GT(entries.size(), descriptorIndex);
  ASSERT_EQ(entries[descriptorIndex]->tag, ConstPool::Utf8);
}
