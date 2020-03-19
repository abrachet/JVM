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

#include "InstructionTests.h"
#include "JVM/VM/Allocator.h"
#include "gtest/gtest.h"

struct Stores : public MethodCaller<> {

  std::string_view getClassName() override { return "General"; }

  void setUpGetAndSetArray() {
    setUpMethod(15);
    EXPECT_EQ(getCode()[0], Instructions::invokestatic);
    EXPECT_EQ(getCode()[3], Instructions::istore_0);
    EXPECT_EQ(getCode()[4], Instructions::return_);
  }
};

TEST_F(Stores, GetAndSetArray) {
  setUpGetAndSetArray();
  while (*getCode() != Instructions::return_)
    tc.callNext();
  EXPECT_EQ(tc.stack.pop<1>(), 5);
  jvm::deallocateAll();
}
