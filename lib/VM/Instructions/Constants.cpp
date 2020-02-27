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

#include "Constants.h"
#include "JVM/Core/BigEndianByteReader.h"
#include "JVM/Core/float_cast.h"
#include <cassert>

template <int size, int toPush> static inline void xconst(ThreadContext &tc) {
  tc.stack.push<size>(toPush);
}

template <int toPush> static inline void iconst(ThreadContext &tc) {
  xconst<1, toPush>(tc);
}

void nop(ThreadContext &tc) { assert(previousInsIs(tc.pc, Instructions::nop)); }

void aconst_null(ThreadContext &tc) { return xconst<2, 0>(tc); }

void iconst_m1(ThreadContext &tc) { return iconst<-1>(tc); }
void iconst_0(ThreadContext &tc) { return iconst<0>(tc); }
void iconst_1(ThreadContext &tc) { return iconst<1>(tc); }
void iconst_2(ThreadContext &tc) { return iconst<2>(tc); }
void iconst_3(ThreadContext &tc) { return iconst<3>(tc); }
void iconst_4(ThreadContext &tc) { return iconst<4>(tc); }
void iconst_5(ThreadContext &tc) { return iconst<5>(tc); }

void lconst_0(ThreadContext &tc) { return xconst<2, 0>(tc); }
void lconst_1(ThreadContext &tc) { return xconst<2, 1>(tc); }

void fconst_0(ThreadContext &tc) {
  constexpr float zero = 0.0;
  return tc.stack.push<1>(float_cast<uint32_t>(zero));
}
void fconst_1(ThreadContext &tc) {
  constexpr float one = 1.0;
  return tc.stack.push<1>(float_cast<uint32_t>(one));
}
void fconst_2(ThreadContext &tc) {
  constexpr float two = 2.0;
  return tc.stack.push<1>(float_cast<uint32_t>(two));
}

void dconst_0(ThreadContext &tc) {
  constexpr double zero = 0.0;
  return tc.stack.push<2>(float_cast<uint64_t>(zero));
}
void dconst_1(ThreadContext &tc) {
  constexpr double one = 1.0;
  return tc.stack.push<2>(float_cast<uint64_t>(one));
}

template <size_t Size> static inline void push(ThreadContext &tc) {
  auto read = readFromPointer<Size>(tc.pc);
  tc.stack.push<Size>(read);
}

void bipush(ThreadContext &tc) { return push<1>(tc); }

void sipush(ThreadContext &tc) { return push<2>(tc); }
