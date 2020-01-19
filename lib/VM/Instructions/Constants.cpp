
#include "Constants.h"
#include "JVM/Core/BigEndianByteReader.h"
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
