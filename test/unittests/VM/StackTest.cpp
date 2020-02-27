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
#include "gtest/gtest.h"

TEST(Stack, Create) {
  ErrorOr<Stack> s = Stack::createStack(4096);
  ASSERT_TRUE(s);
  ASSERT_NE(s->stack, nullptr);
  ASSERT_EQ(s->stackOverflowCallback, nullptr);
  char *start = reinterpret_cast<char *>(s->stack);
  EXPECT_EQ(start + 4096, s->sp);
}

TEST(Stack, Push) {
  ErrorOr<Stack> s = Stack::createStack(4096);
  ASSERT_TRUE(s);
  uintptr_t start = (uintptr_t)((char *)(s->stack) + 4096);
  auto sp = [&] { return reinterpret_cast<uintptr_t>(s->sp); };
  EXPECT_EQ(sp(), start);
  s->push<1>(58);
  EXPECT_EQ(sp(), start - 4);
  uint32_t *i = reinterpret_cast<uint32_t *>(s->stack);
  uint64_t *l = reinterpret_cast<uint64_t *>(s->stack);
  EXPECT_EQ(i[1023], 58);
  s->push<1>(37);
  EXPECT_EQ(sp(), start - 8);
  EXPECT_EQ(i[1022], 37);
  s->push<2>(93428124);
  EXPECT_EQ(sp(), start - 16);
  EXPECT_EQ(l[510], 93428124);
}

TEST(Stack, StackOverflow) {
  bool b = false;
  std::function<void(Stack &)> callback = [&b](Stack &) { b = true; };
  ErrorOr<Stack> s = Stack::createStack(4096, callback);
  ASSERT_TRUE(s);
  ASSERT_NE(s->stackOverflowCallback, nullptr);
  void *saveStack = s->stack;
  s->stack = reinterpret_cast<char *>(s->stack) + 4092;
  s->push<1>(0);
  EXPECT_FALSE(b);
  s->push<1>(0);
  EXPECT_TRUE(b);
  s->stack = saveStack;
}

TEST(Stack, Pop) {
  ErrorOr<Stack> s = Stack::createStack(4096);
  ASSERT_TRUE(s);
  s->push<1>(48);
  uint64_t i = s->pop<1>();
  EXPECT_EQ(i, 48);
  EXPECT_EQ(s->sp, (const char *)s->stack + s->size);
}
