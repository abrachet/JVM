
#include "JVM/VM/Instructions.h"
#include <cassert>

#include "Constants/Constants.h"

#define unimplemented(str)                                                     \
  [](ThreadContext &) -> void {                                                \
    assert(0 && "Instruction '" #str "' is not yet implemented");              \
  }

InsT instructions[256] = {
    [Instructions::nop] = nop,
    [Instructions::aconst_null] = unimplemented("aconst_null"),
};
