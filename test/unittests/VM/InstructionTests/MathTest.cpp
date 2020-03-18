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
#include "gtest/gtest.h"

struct Math : public InstructionTest {
  uint8_t instruction;
  template <typename T> void test(T left, T right, T expected) {
    constexpr size_t stackEntrySize = sizeof(T) / Stack::stackEntryBytes;
    threadContext->pc = &instruction;
    threadContext->stack.push<stackEntrySize>(left);
    threadContext->stack.push<stackEntrySize>(right);
    threadContext->callNext();
    EXPECT_EQ((T)threadContext->stack.pop<stackEntrySize>(), expected);
  }
};

TEST_F(Math, IAdd) {
  instruction = Instructions::iadd;
  test(5, 1, 6);
  test(std::numeric_limits<int32_t>::max(), 1,
       std::numeric_limits<int32_t>::min());
  test(-1, 1, 0);
}

TEST_F(Math, LAdd) {
  instruction = Instructions::ladd;
  test(5L, 1L, 6L);
  test<int64_t>(std::numeric_limits<int32_t>::max(), 1,
                (uint64_t)std::numeric_limits<int32_t>::max() + 1);
  test<int64_t>(-1, 1, 0);
}

TEST_F(Math, ISub) {
  instruction = Instructions::isub;
  test(5, 1, 4);
  test(0, 1, -1);
  test(1, 1, 0);
}

TEST_F(Math, LSub) {
  instruction = Instructions::lsub;
  test(5L, 1L, 4L);
  test(10000000000L, 1L, 9999999999L);
  test(1L, 1L, 0L);
  test(0L, 1L, -1L);
}

TEST_F(Math, IMul) {
  instruction = Instructions::imul;
  test(5, 1, 5);
  test(5, 2, 10);
  test(0, 10, 0);
  test(10, -1, -10);
  test(-1, -5, 5);
  test(std::numeric_limits<int32_t>::max(), 2, -2);
}

TEST_F(Math, LMul) {
  instruction = Instructions::lmul;
  test(5L, 1L, 5L);
  test(0L, 10L, 0L);
  test(10L, -1L, -10L);
  test<int64_t>(std::numeric_limits<int32_t>::max(), 2, 4294967294);
}

TEST_F(Math, IInc) {
  uint8_t ins[] = {
      Instructions::iconst_0, Instructions::istore_0, Instructions::iinc, 0, 1,
      Instructions::iload_0,
  };
  threadContext->pushFrame("<test>");
  threadContext->pc = ins;
  threadContext->stack.push<1>(10001);
  for (int i = 0; i < 4; i++)
    threadContext->callNext();
  EXPECT_EQ(threadContext->stack.pop<1>(), 1);
}
