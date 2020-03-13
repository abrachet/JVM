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

#include "JVM/VM/Allocator.h"
#include "JVM/VM/Class.h"
#include "JVM/VM/InMemoryObject.h"
#include <cstdlib>
#include <cstring>

InMemoryObject *jvm::allocate(const jvm::Class &clss) {
  size_t objectSize = clss.objectRepresentation.getObjectSize();
  size_t aligned = ((sizeof(InMemoryObject) + objectSize) +
                    (jvm::requiredTypeAlignment - 1)) &
                   (-jvm::requiredTypeAlignment);
  void *const mem = aligned_alloc(jvm::requiredTypeAlignment, aligned);
  assert(mem);
  new (mem) InMemoryObject{clss};
  std::memset(reinterpret_cast<char *>(mem) + sizeof(InMemoryObject), 0,
              objectSize);
  assert(!(reinterpret_cast<uintptr_t>(reinterpret_cast<char *>(mem) +
                                       sizeof(InMemoryObject)) %
           jvm::requiredTypeAlignment));
  return reinterpret_cast<InMemoryObject *>(mem);
}

void jvm::deallocate(InMemoryObject *ptr) { std::free(ptr); }
