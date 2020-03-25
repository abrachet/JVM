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

class ClassReaderException : public ClassReaderBase {
  const char *getFilename() const override { return "ExceptionTable.class"; }
};

TEST_F(ClassReaderException, Basic) {
  const auto &cp = classFile->getConstPool();
  const auto &methods = classFile->getMethods();
  ErrorOr<Class::CodeAttribute> codeOrErr = methods[2].findCodeAttr(cp);
  ASSERT_TRUE(codeOrErr);
  const Class::CodeAttribute &code = *codeOrErr;

  auto getClassName = [&cp](int classInfoIndex) -> std::string_view {
    const auto &classInfo = cp.get<Class::ConstPool::ClassInfo>(classInfoIndex);
    return cp.get<Class::ConstPool::Utf8Info>(classInfo.nameIndex);
  };

  EXPECT_EQ(code.exceptionTableLength, 2);

  const auto &firstEntry = code.exceptionTable[0];
  EXPECT_EQ(firstEntry.startPc, 0);
  EXPECT_EQ(firstEntry.endPc, 3);
  EXPECT_EQ(firstEntry.handlerPc, 6);
  EXPECT_EQ(getClassName(firstEntry.catchType), "java/lang/Exception");

  const auto &secondEntry = code.exceptionTable[1];
  EXPECT_EQ(secondEntry.startPc, 0);
  EXPECT_EQ(secondEntry.endPc, 3);
  EXPECT_EQ(secondEntry.handlerPc, 9);
  EXPECT_EQ(getClassName(secondEntry.catchType), "java/lang/Throwable");
}
