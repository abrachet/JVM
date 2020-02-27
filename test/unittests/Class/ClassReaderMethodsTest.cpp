// Copyright 2020 Alex Brachet
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "JVM/Class/ClassFileReader.h"
#include "gtest/gtest.h"
#include <cstring>

#include "ClassReaderTest.h"

using namespace Class;

class ClassReaderMethods : public ClassReaderBase {
  const char *getFilename() const override { return "Methods.class"; }
};

using Entry = std::unique_ptr<Class::ConstPool::ConstPoolBase>;

static void assertCPStreq(const char *str, const Entry &entry) {
  ASSERT_EQ(entry->tag, ConstPool::Utf8);
  auto &utf8 = *reinterpret_cast<const ConstPool::Utf8Info *>(entry.get());
  ASSERT_EQ(strlen(str), utf8.length);
  EXPECT_FALSE(
      strncmp(reinterpret_cast<const char *>(utf8.bytes), str, utf8.length));
}

TEST_F(ClassReaderMethods, Basic) {
  auto &methods = classFile->getMethods();
  auto &cpEntries = classFile->getConstPool().getEntries();
  ASSERT_EQ(methods.size(), 6);
  assertCPStreq("<init>", cpEntries[methods[0].nameIndex]);
  assertCPStreq("pubInt", cpEntries[methods[1].nameIndex]);
  assertCPStreq("privFloat", cpEntries[methods[2].nameIndex]);
  assertCPStreq("protChar", cpEntries[methods[3].nameIndex]);
  assertCPStreq("retMethods", cpEntries[methods[4].nameIndex]);
  assertCPStreq("retObject", cpEntries[methods[5].nameIndex]);
}

TEST_F(ClassReaderMethods, init) {
  auto &methods = classFile->getMethods();
  auto &cpEntries = classFile->getConstPool().getEntries();
  ASSERT_EQ(methods[0].attributeCount, 1);
  int attrNameIdx = methods[0].attributes[0].attributeNameIndex;
  assertCPStreq("Code", cpEntries[attrNameIdx]);
  int descIdx = methods[0].descriptorIndex;
  assertCPStreq("()V", cpEntries[descIdx]);
  ASSERT_EQ(methods[0].accessFlags, 0);
}

TEST_F(ClassReaderMethods, pubInt) {
  auto &methods = classFile->getMethods();
  auto &cpEntries = classFile->getConstPool().getEntries();
  ASSERT_EQ(methods[1].attributeCount, 1);
  int attrNameIdx = methods[1].attributes[0].attributeNameIndex;
  assertCPStreq("Code", cpEntries[attrNameIdx]);
  int descIdx = methods[1].descriptorIndex;
  assertCPStreq("()I", cpEntries[descIdx]);
  ASSERT_EQ(methods[1].accessFlags, Class::Method::AccessFlags::Public);
}

TEST_F(ClassReaderMethods, privFloat) {
  auto &methods = classFile->getMethods();
  auto &cpEntries = classFile->getConstPool().getEntries();
  ASSERT_EQ(methods[2].attributeCount, 1);
  int attrNameIdx = methods[2].attributes[0].attributeNameIndex;
  assertCPStreq("Code", cpEntries[attrNameIdx]);
  int descIdx = methods[2].descriptorIndex;
  assertCPStreq("()F", cpEntries[descIdx]);
  ASSERT_EQ(methods[2].accessFlags, Class::Method::AccessFlags::Private);
}

TEST_F(ClassReaderMethods, protChar) {
  auto &methods = classFile->getMethods();
  auto &cpEntries = classFile->getConstPool().getEntries();
  ASSERT_EQ(methods[3].attributeCount, 1);
  int attrNameIdx = methods[3].attributes[0].attributeNameIndex;
  assertCPStreq("Code", cpEntries[attrNameIdx]);
  int descIdx = methods[3].descriptorIndex;
  assertCPStreq("()C", cpEntries[descIdx]);
  ASSERT_EQ(methods[3].accessFlags, Class::Method::AccessFlags::Protected);
}

TEST_F(ClassReaderMethods, retMethods) {
  auto &methods = classFile->getMethods();
  auto &cpEntries = classFile->getConstPool().getEntries();
  ASSERT_EQ(methods[4].attributeCount, 1);
  int attrNameIdx = methods[4].attributes[0].attributeNameIndex;
  assertCPStreq("Code", cpEntries[attrNameIdx]);
  int descIdx = methods[4].descriptorIndex;
  assertCPStreq("()LMethods;", cpEntries[descIdx]);
  ASSERT_EQ(methods[4].accessFlags, Class::Method::AccessFlags::Public);
}

TEST_F(ClassReaderMethods, retObject) {
  auto &methods = classFile->getMethods();
  auto &cpEntries = classFile->getConstPool().getEntries();
  ASSERT_EQ(methods[5].attributeCount, 1);
  int attrNameIdx = methods[5].attributes[0].attributeNameIndex;
  assertCPStreq("Code", cpEntries[attrNameIdx]);
  int descIdx = methods[5].descriptorIndex;
  assertCPStreq("()Ljava/lang/Object;", cpEntries[descIdx]);
  ASSERT_EQ(methods[5].accessFlags, Class::Method::AccessFlags::Public);
}

TEST_F(ClassReaderMethods, FindMethod) {
  // ConstPool entry #2 is MethodRef to Methods.retMethods:()LMethods;
  // Method index 4 refers to this method. Too lazy to find these
  // programatically in CMakeLists.txt.
  auto &constPool = classFile->getConstPool();
  auto &methodRef = constPool.get<ConstPool::MethodrefInfo>(2);
  auto &methods = classFile->getMethods();
  ErrorOr<const Class::Method &> methodOrErr =
      classFile->findStaticMethod(methodRef);
  ASSERT_TRUE(methodOrErr);
  EXPECT_EQ(&*methodOrErr, &methods[4]);
}
