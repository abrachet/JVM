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

struct Math : public InstructionTest {};

TEST_F(Math, IAdd) {
  uint8_t instructions[] = {Instructions::iadd, 0};
  threadContext->pc = instructions;
  threadContext->stack.push<1>(5);
  threadContext->stack.push<1>(1);
  threadContext->callNext();
  EXPECT_EQ(threadContext->stack.pop<1>(), 6);

  threadContext->pc = instructions;
  threadContext->stack.push<1>(std::numeric_limits<int32_t>::max());
  threadContext->stack.push<1>(1);
  threadContext->callNext();
  EXPECT_EQ((int32_t)threadContext->stack.pop<1>(),
            std::numeric_limits<int32_t>::min());

  threadContext->pc = instructions;
  threadContext->stack.push<1>(int32_t(-1));
  threadContext->stack.push<1>(1);
  threadContext->callNext();
  EXPECT_EQ(threadContext->stack.pop<1>(), 0);
}
