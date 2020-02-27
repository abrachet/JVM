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

#ifndef JVM_VM_OBJECTS_H
#define JVM_VM_OBJECTS_H

#include "JVM/Class/ClassFile.h"
#include "JVM/Core/ErrorOr.h"
#include "JVM/VM/Type.h"
#include "JVM/string_view"
#include <vector>

namespace jvm {

using jfieldID = unsigned;

}

class ObjectRepresentation {
  std::vector<std::pair<Type, uint64_t>> memberFields;
  uint64_t size;

public:
  ObjectRepresentation() = default;
  static ErrorOr<ObjectRepresentation> createFromClassFile(const ClassFile &);

  uint64_t getObjectSize() const { return size; }
  uint64_t getFieldOffset(jvm::jfieldID id) const {
    return memberFields[id].second;
  }
};

#endif // JVM_VM_OBJECTS_H
