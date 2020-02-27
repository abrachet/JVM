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
#include <fstream>
#include <unistd.h>

#include "ClassReaderTest.h"

class ClassReaderInterface : public ClassReaderBase {
  const char *getFilename() const override { return "Interface.class"; }
};

class ClassReaderImplements : public ClassReaderBase {
  const char *getFilename() const override { return "C.class"; }
};

constexpr int numCPEntries =
#include "Interface_NumCPEntries.inc"
    ;

TEST_F(ClassReaderInterface, NumConstPoolEntries) {
  auto &entries = classFile->getConstPool().getEntries();
  // Add 1, if the last number seen is 6, the number of entries is 7
  EXPECT_EQ(entries.size(), numCPEntries + 1);
}

constexpr int numInterfaces =
#include "Interface_NumInterface.inc"
    ;

TEST_F(ClassReaderImplements, Basic) {
  if (numInterfaces != -1)
    EXPECT_EQ(classFile->getInterfaces().size(), numInterfaces);
}
