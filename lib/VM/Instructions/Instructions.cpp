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
#include <array>
#include <cassert>

#include "Constants.h"
#include "Control.h"
#include "Loads.h"
#include "References.h"
#include "Stack.h"

#define unimplemented(str)                                                     \
  +[](ThreadContext &) -> void {                                               \
    assert(0 && "Instruction '" #str "' is not yet implemented");              \
  }

std::array<InsT, 256> instructions = []() constexpr {
  std::array<InsT, 256> array;
  array[Instructions::nop] = nop;
  array[Instructions::aconst_null] = aconst_null;
  array[Instructions::iconst_m1] = iconst_m1;
  array[Instructions::iconst_0] = iconst_0;
  array[Instructions::iconst_1] = iconst_1;
  array[Instructions::iconst_2] = iconst_2;
  array[Instructions::iconst_3] = iconst_3;
  array[Instructions::iconst_4] = iconst_4;
  array[Instructions::iconst_5] = iconst_5;
  array[Instructions::lconst_0] = lconst_0;
  array[Instructions::lconst_1] = lconst_1;
  array[Instructions::fconst_0] = fconst_0;
  array[Instructions::fconst_1] = fconst_1;
  array[Instructions::fconst_2] = fconst_2;
  array[Instructions::dconst_0] = dconst_0;
  array[Instructions::dconst_1] = dconst_1;
  array[Instructions::bipush] = bipush;
  array[Instructions::sipush] = sipush;

  // Loads
  array[Instructions::iload_0] = iload_0;
  array[Instructions::iload_1] = iload_1;
  array[Instructions::iload_2] = iload_2;
  array[Instructions::iload_3] = iload_3;

  // Stores
  // This is just temporary to get the loads test to work.
  array[Instructions::istore_0] =
      +[](ThreadContext &tc) { tc.storeInLocal<1>(0, tc.stack.pop<1>()); };
  array[Instructions::istore_1] =
      +[](ThreadContext &tc) { tc.storeInLocal<1>(1, tc.stack.pop<1>()); };

  // Stack
  array[Instructions::dup] = Ins::dup;
  // Math

  // Conversions

  // Comparasions

  // Control
  array[Instructions::ireturn] = ireturn;
  array[Instructions::return_] = return_;

  // References
  array[Instructions::invokestatic] = invokestatic;
  array[Instructions::new_] = new_;

  // Extended

  // Reserved
  return array;
}
();
