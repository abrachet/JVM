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

#ifndef INS_MATH_H
#define INS_MATH_H

#include "Instruction.h"
#include "JVM/Core/BigEndianByteReader.h"
#include "JVM/VM/Instructions.h"
#include <functional>

template <size_t Width, typename BinaryFunc>
static void mathOperation(ThreadContext &tc) {
  using EntryT = Stack::EntryType<Width>;
  BinaryFunc func;
  EntryT right = tc.stack.pop<Width>();
  EntryT left = tc.stack.pop<Width>();
  tc.stack.push<Width>(func(left, right));
}

static void iadd(ThreadContext &tc) {
  return mathOperation<1, std::plus<>>(tc);
}
static void ladd(ThreadContext &tc) {
  return mathOperation<2, std::plus<>>(tc);
}

static void isub(ThreadContext &tc) {
  return mathOperation<1, std::minus<>>(tc);
}
static void lsub(ThreadContext &tc) {
  return mathOperation<2, std::minus<>>(tc);
}

static void imul(ThreadContext &tc) {
  return mathOperation<1, std::multiplies<>>(tc);
}
static void lmul(ThreadContext &tc) {
  return mathOperation<2, std::multiplies<>>(tc);
}

static void iinc(ThreadContext &tc) {
  uint8_t local = readFromPointer<uint8_t>(tc.pc);
  uint32_t *var = reinterpret_cast<uint32_t *>(tc.getAddressOfLocal<1>(local));
  *var += readFromPointer<uint8_t>(tc.pc);
}

#endif // INS_LOADS_H
