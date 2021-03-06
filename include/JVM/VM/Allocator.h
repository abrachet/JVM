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

#ifndef JVM_VM_ALLOCATOR_H
#define JVM_VM_ALLOCATOR_H

#include "JVM/VM/Class.h"
#include "JVM/VM/ClassLoader.h"
#include "JVM/VM/InMemoryObject.h"
#include "JVM/VM/Type.h"

namespace jvm {

[[nodiscard]] uint32_t allocate(const jvm::Class &);
[[nodiscard]] uint32_t allocateArray(Type type, size_t length);
[[nodiscard]] static constexpr uint32_t nullref() { return 0; }

InMemoryItem *getAllocatedItem(uint32_t);

void deallocate(uint32_t);

size_t getNumAllocated();

void deallocateAll();

} // namespace jvm

#endif // JVM_VM_ALLOCATOR_H