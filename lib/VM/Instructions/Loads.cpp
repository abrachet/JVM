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

#include "Loads.h"
#include "JVM/Core/BigEndianByteReader.h"
#include "JVM/VM/Allocator.h"
#include "JVM/VM/InMemoryObject.h"

template <typename T> static void load(ThreadContext &tc) {
  uint32_t index = tc.stack.pop<1>();
  uint32_t objKey = tc.stack.pop<1>();
  InMemoryArray *array =
      reinterpret_cast<InMemoryArray *>(jvm::getAllocatedItem(objKey));
  assert(index < array->length && "IndexOutOfBoundsException");
  T *underlyingArr = reinterpret_cast<T *>(array->getThisptr());
  constexpr size_t stackEntryCount = sizeof(T) / Stack::stackEntryBytes;
  tc.stack.push<stackEntryCount>(underlyingArr[index]);
}

void iaload(ThreadContext &tc) { return load<int32_t>(tc); }
