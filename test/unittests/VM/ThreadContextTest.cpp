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

#include "JVM/VM/ThreadContext.h"
#include "gtest/gtest.h"

TEST(ThreadContext, LoadLocalVariable) {
  auto stackOrErr = Stack::createStack(4096);
  ASSERT_TRUE(stackOrErr);
  ThreadContext tc(std::move(*stackOrErr));
  tc.pushFrame("<test>");
  tc.stack.push<1>(10);
  tc.stack.push<2>(100);
  uint64_t iload0 = tc.loadFromLocal<1>(0);
  EXPECT_EQ(iload0, 10);
  uint64_t lload1 = tc.loadFromLocal<2>(1);
  EXPECT_EQ(lload1, 100);
}

TEST(ThreadContext, StoreLocalVariable) {
  auto stackOrErr = Stack::createStack(4096);
  ASSERT_TRUE(stackOrErr);
  ThreadContext tc(std::move(*stackOrErr));
  tc.pushFrame("<test>");
  tc.stack.push<1>(10);
  uint64_t got = tc.stack.pop<1>();
  ASSERT_EQ(got, 10);
  tc.stack.push<1>(56); // Make room for local.
  tc.storeInLocal<1>(0, got);
  got = tc.stack.pop<1>();
  ASSERT_EQ(got, 10);
}