
#include "Constants.h"
#include "JVM/Core/BigEndianByteReader.h"
#include <cassert>

void nop(ThreadContext &tc) { assert(previousInsIs(tc.pc, Instructions::nop)); }
