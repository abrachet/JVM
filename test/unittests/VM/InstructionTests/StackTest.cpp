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

#include "JVM/VM/Stack.h"
#include "InstructionTests.h"
#include "JVM/VM/Instructions.h"
#include "JVM/VM/ThreadContext.h"
#include "gtest/gtest.h"

struct StackTest : public MethodCaller<> {

  std::string_view getClassName() override { return "General"; }

  void setUpTestDup() {
    setUpMethod(7);
    EXPECT_EQ(getCode()[0], Instructions::new_);
    EXPECT_EQ(getCode()[3], Instructions::dup);
  }
};

TEST_F(StackTest, Dup) {
  setUpTestDup();
  callMultiple(2);
  uint32_t first = tc.stack.pop<1>(), second = tc.stack.pop<1>();
  EXPECT_EQ(first, second);
}
