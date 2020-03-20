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

#include "JVM/VM/ClassHierarchyWalker.h"
#include "JVM/VM/Class.h"

ClassHierarchyWalker::ClassAndMethod
ClassHierarchyWalker::findVirtualMethod(std::string_view methodName,
                                        std::string_view typeName) const {
  const jvm::Class *curr = std::addressof(current);
  for (; curr->superClasses.size();
       curr = std::addressof(curr->superClasses[0].get())) {
    ErrorOr<const Class::Method &> methodOrErr =
        curr->loadedClass->findMethodByNameType(methodName, typeName);
    if (methodOrErr)
      return {*curr, *methodOrErr};
  }
  assert(0 && "IncompatibleClassChangeError");
}

bool ClassHierarchyWalker::extends(const jvm::Class &curr,
                                   std::string_view name) const {
  if (curr.name == name)
    return true;
  for (const jvm::Class &c : curr.superClasses) {
    if (extends(c, name))
      return true;
  }
  return false;
}

bool ClassHierarchyWalker::extends(std::string_view className) const {
  return extends(current, className);
}
