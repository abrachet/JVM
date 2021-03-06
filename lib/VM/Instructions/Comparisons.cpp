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

#include "Comparisons.h"
#include "JVM/Core/BigEndianByteReader.h"
#include <algorithm>
#include <cassert>

template <typename Compare> static void ifcmp(ThreadContext &tc) {
  int16_t branch = readFromPointer<int16_t>(tc.pc);
  branch -= 3;
  uint32_t cmp = tc.stack.pop<1>();
  Compare comp;
  if (comp(cmp, 0))
    tc.jump(branch);
}

void ifeq(ThreadContext &tc) { return ifcmp<std::equal_to<>>(tc); }

void ifne(ThreadContext &tc) { return ifcmp<std::not_equal_to<>>(tc); }

template <typename Compare> static void if_cmp(ThreadContext &tc) {
  int16_t branch = readFromPointer<int16_t>(tc.pc);
  // Currently 3 bytes from goto instruction [goto, branch 1st, branch 2nd]
  branch -= 3;
  uint32_t right = tc.stack.pop<1>();
  uint32_t left = tc.stack.pop<1>();
  Compare cmp;
  if (cmp(left, right))
    tc.jump(branch);
}

void if_icmpeq(ThreadContext &tc) { return if_cmp<std::equal_to<>>(tc); }

void if_icmpne(ThreadContext &tc) { return if_cmp<std::not_equal_to<>>(tc); }
