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
#include "JVM/VM/InMemoryObject.h"
#include "JVM/VM/Instructions.h"
#include "JVM/VM/Stack.h"
#include "JVM/VM/ThreadContext.h"
#include "gtest/gtest.h"

struct New : public MethodCaller<> {

  std::string_view getClassName() override { return "General"; }

  void setUpTestNew() {
    setUpMethod(7);
    EXPECT_EQ(getCode()[0], Instructions::new_);
    EXPECT_EQ(getCode()[3], Instructions::dup);
  }

  void setUpCallGetArrayLength() {
    setUpMethod(13);
    EXPECT_EQ(getCode()[0], Instructions::invokestatic);
    EXPECT_EQ(getCode()[3], Instructions::istore_0);
    EXPECT_EQ(getCode()[4], Instructions::return_);
  }
};

TEST_F(New, Basic) {
  setUpTestNew();
  tc.callNext();
  uint32_t objRef = tc.stack.pop<1>();
  InMemoryItem *ptr = jvm::getAllocatedItem(objRef);
  EXPECT_EQ(reinterpret_cast<InMemoryObject *>(ptr)->getName(), "General");
  jvm::deallocate(objRef);
}

TEST_F(New, NewArray) {
  setUpCallGetArrayLength();
  while (*getCode() != Instructions::return_)
    tc.callNext();
  EXPECT_EQ(tc.stack.pop<1>(), 10);
  jvm::deallocateAll();
}
