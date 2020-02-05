
#ifndef INS_LOADS_H
#define INS_LOADS_H

#include "Instruction.h"
#include "JVM/VM/Instructions.h"

template <size_t LoadWidth, size_t Index> void load(ThreadContext &tc) {
  tc.stack.push<LoadWidth>(tc.loadFromLocal<LoadWidth>(Index));
}

template <size_t Index> void iload(ThreadContext &tc) { load<1, Index>(tc); }

static void iload_0(ThreadContext &tc) { iload<0>(tc); }
static void iload_1(ThreadContext &tc) { iload<1>(tc); }
static void iload_2(ThreadContext &tc) { iload<2>(tc); }
static void iload_3(ThreadContext &tc) { iload<3>(tc); }

#endif // INS_LOADS_H