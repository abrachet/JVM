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

#ifndef JVM_VM_CLASSHIERARCHYWALKER_H
#define JVM_VM_CLASSHIERARCHYWALKER_H

#include "JVM/Class/ClassFile.h"
#include "JVM/Core/ErrorOr.h"
#include "JVM/VM/Class.h"
#include "JVM/string_view"
#include <utility>

class ClassHierarchyWalker {
  const jvm::Class &current;

  bool extends(const jvm::Class &, std::string_view) const;

public:
  ClassHierarchyWalker(const jvm::Class &clss) : current(clss) {}

  using ClassAndMethod = std::pair<const jvm::Class &, const Class::Method &>;
  ClassAndMethod findVirtualMethod(std::string_view methodName,
                                   std::string_view typeName) const;

  bool extends(std::string_view className) const;
};

#endif // JVM_VM_CLASSHIERARCHYWALKER_H
