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

#include "JVM/VM/Instructions.h"
#include <cassert>

#include "Constants.h"
#include "Control.h"
#include "Loads.h"
#include "References.h"

#define unimplemented(str)                                                     \
  [](ThreadContext &) -> void {                                                \
    assert(0 && "Instruction '" #str "' is not yet implemented");              \
  }

InsT instructions[256] = {
    // Constants
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
    [Instructions::bipush] = bipush,
    [Instructions::sipush] = sipush,

    // Loads
    [Instructions::iload_0] = iload_0,
    [Instructions::iload_1] = iload_1,
    [Instructions::iload_2] = iload_2,
    [Instructions::iload_3] = iload_3,

    // Stores
    // This is just temporary to get the loads test to work.
    [Instructions::istore_0] =
        [](ThreadContext &tc) { tc.storeInLocal<1>(0, tc.stack.pop<1>()); },
    [Instructions::istore_1] =
        [](ThreadContext &tc) { tc.storeInLocal<1>(1, tc.stack.pop<1>()); },

    // Stack

    // Math

    // Conversions

    // Comparasions

    // Control
    [Instructions::ireturn] = ireturn,

    // References
    [Instructions::invokestatic] = invokestatic,

    // Extended

    // Reserved
};
