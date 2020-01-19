
#include "Constants.h"
#include "JVM/Core/BigEndianByteReader.h"
#include <cassert>

void nop(ThreadContext &tc) { assert(previousInsIs(tc.pc, Instructions::nop)); }

template <int toPush> void iconst(ThreadContext &tc) {
  tc.stack.push<1>(toPush);
}

void iconst_m1(ThreadContext &tc) { return iconst<-1>(tc); }

void iconst_0(ThreadContext &tc) { return iconst<0>(tc); }

void iconst_1(ThreadContext &tc) { return iconst<1>(tc); }

void iconst_2(ThreadContext &tc) { return iconst<2>(tc); }

void iconst_3(ThreadContext &tc) { return iconst<3>(tc); }

void iconst_4(ThreadContext &tc) { return iconst<4>(tc); }

void iconst_5(ThreadContext &tc) { return iconst<5>(tc); }
