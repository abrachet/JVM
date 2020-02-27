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

namespace jvm {

InMemoryObject *allocate(const jvm::Class &);

void deallocate(InMemoryObject *);

} // namespace jvm

#endif // JVM_VM_ALLOCATOR_H