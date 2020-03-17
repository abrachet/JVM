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
  test<uint64_t>(std::numeric_limits<int32_t>::max(), 1,
                 (uint64_t)std::numeric_limits<int32_t>::max() + 1);
  test<int64_t>(-1, 1, 0);
}
