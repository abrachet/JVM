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

#include "JVM/VM/ObjectRepresentation.h"
#include "JVM/VM/Stack.h"

ErrorOr<ObjectRepresentation>
ObjectRepresentation::createFromClassFile(const ClassFile &classFile) {
  const auto &constPool = classFile.getConstPool();
  auto getType = [&constPool](int nameIndex) {
    std::string_view sv = constPool.get<Class::ConstPool::Utf8Info>(nameIndex);
    ErrorOr<Type> typeOrErr = Type::parseType(sv);
    assert(typeOrErr && "Couldn't parse type");
    return *typeOrErr;
  };
  constexpr auto oneEntrySize = Stack::stackEntryBytes;
  constexpr auto twoEntrySize = 2 * oneEntrySize;

  ObjectRepresentation OR;
  size_t currentOffset = 0;
  for (const auto &field : classFile.getFields()) {
    if (field.accessFlags & Class::Field::Static)
      continue;
    Type t = getType(field.descriptorIndex);
    size_t fieldSize = t.getStackEntryCount() * oneEntrySize;
    if (currentOffset % fieldSize) {
      currentOffset += oneEntrySize;
      assert(!(currentOffset % twoEntrySize) && "bad alignment");
    }
    OR.memberFields.emplace_back(t, currentOffset);
    currentOffset += fieldSize;
  }
  OR.size = currentOffset;
  return OR;
}
