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
#include <string>

#include "ClassReaderTest.h"

using namespace Class;
using namespace std::string_literals;

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
  const auto &constPool = classFile->getConstPool();
  const Field &testString = fields[0];
  auto &utf8 = constPool.get<ConstPool::Utf8Info>(testString.nameIndex);
  EXPECT_EQ("testString"s, std::string(utf8));
}

TEST_F(ClassReaderFields, Descriptor) {
  const auto &fields = classFile->getFields();
  const auto &constPool = classFile->getConstPool();
  const Field &testString = fields[0];
  auto &utf8 = constPool.get<ConstPool::Utf8Info>(testString.descriptorIndex);
  EXPECT_EQ("Ljava/lang/String;"s, std::string(utf8));

  const Field &testInt = fields[1];
  auto &utf82 = constPool.get<ConstPool::Utf8Info>(testInt.descriptorIndex);
  EXPECT_EQ("I"s, std::string(utf82));
}

// TODO: test attributes
