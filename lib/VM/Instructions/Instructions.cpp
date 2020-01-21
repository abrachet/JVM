
#include "JVM/VM/Instructions.h"
#include <cassert>

#include "Constants.h"
#include "References.h"

#define unimplemented(str)                                                     \
  [](ThreadContext &) -> void {                                                \
    assert(0 && "Instruction '" #str "' is not yet implemented");              \
  }

InsT instructions[256] = {
    [Instructions::nop] = nop,
    [Instructions::aconst_null] = aconst_null,
    [Instructions::iconst_m1] = iconst_m1,
    [Instructions::iconst_0] = iconst_0,
    [Instructions::iconst_1] = iconst_1,
    [Instructions::iconst_2] = iconst_2,
    [Instructions::iconst_3] = iconst_3,
    [Instructions::iconst_4] = iconst_4,
    [Instructions::iconst_5] = iconst_5,
    [Instructions::lconst_0] = lconst_0,
    [Instructions::lconst_1] = lconst_1,
    [Instructions::fconst_0] = fconst_0,
    [Instructions::fconst_1] = fconst_1,
    [Instructions::fconst_2] = fconst_2,
    [Instructions::dconst_0] = dconst_0,
    [Instructions::dconst_1] = dconst_1,

    [Instructions::invokestatic] = invokestatic,
};
