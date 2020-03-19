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

#include "Stores.h"
#include "JVM/Core/BigEndianByteReader.h"
#include "JVM/VM/Allocator.h"
#include "JVM/VM/InMemoryObject.h"
#include "JVM/VM/Stack.h"

template <typename T> static void astore(ThreadContext &tc) {
  constexpr size_t stackEntrySize = sizeof(T) / Stack::stackEntryBytes;
  static_assert(stackEntrySize == 1);
  uint64_t toStore = tc.stack.pop<stackEntrySize>();
  uint32_t index = tc.stack.pop<1>();
  uint32_t objKey = tc.stack.pop<1>();
  InMemoryArray *array =
      reinterpret_cast<InMemoryArray *>(jvm::getAllocatedItem(objKey));
  assert(index < array->length && "IndexOutOfBoundsException");
  T *underlyingArr = reinterpret_cast<T *>(array->getThisptr());
  underlyingArr[index] = toStore;
}

void iastore(ThreadContext &tc) { return astore<int32_t>(tc); }
