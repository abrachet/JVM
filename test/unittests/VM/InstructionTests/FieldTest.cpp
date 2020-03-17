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
#include "JVM/VM/ClassLoader.h"
#include "JVM/VM/Instructions.h"
#include "JVM/VM/Stack.h"
#include "JVM/VM/ThreadContext.h"
#include "gtest/gtest.h"

struct Field : public MethodCaller<> {

  std::string_view getClassName() override { return "General"; }

  void setUpReturnDefault() {
    setUpMethod(8);
    EXPECT_EQ(getCode()[0], Instructions::new_);
    EXPECT_EQ(getCode()[3], Instructions::dup);
    EXPECT_EQ(getCode()[4], Instructions::invokespecial);
    EXPECT_EQ(getCode()[7], Instructions::astore_0);
    EXPECT_EQ(getCode()[8], Instructions::aload_0);
    EXPECT_EQ(getCode()[9], Instructions::getfield);
  }

  void setUpSetAndReturn() {
    setUpMethod(9);
    EXPECT_EQ(getCode()[0], Instructions::new_);
    EXPECT_EQ(getCode()[3], Instructions::dup);
    EXPECT_EQ(getCode()[4], Instructions::invokespecial);
    EXPECT_EQ(getCode()[7], Instructions::astore_0);
    EXPECT_EQ(getCode()[8], Instructions::aload_0);
    EXPECT_EQ(getCode()[9], Instructions::iconst_5);
    EXPECT_EQ(getCode()[10], Instructions::putfield);
  }
};

TEST_F(Field, Getfield) {
  setUpReturnDefault();
  EXPECT_EQ(jvm::getNumAllocated(), 0);
  tc.callNext();
  EXPECT_EQ(jvm::getNumAllocated(), 1);
  callMultiple(5);
  uint32_t onStack = tc.stack.pop<1>();
  EXPECT_EQ(onStack, 0);
  uint32_t obj = tc.stack.pop<1>();
  jvm::deallocate(obj);
}

TEST_F(Field, GetSetField) {
  setUpSetAndReturn();
  EXPECT_EQ(jvm::getNumAllocated(), 0);
  // Call before return
  callMultiple(9);
  EXPECT_EQ(jvm::getNumAllocated(), 1);
  uint32_t onStack = tc.stack.pop<1>();
  EXPECT_EQ(onStack, 5);
  uint32_t obj = tc.stack.pop<1>();
  jvm::deallocate(obj);
}
