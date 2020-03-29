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

#ifndef JVM_VM_CLASS_H
#define JVM_VM_CLASS_H

#include "JVM/Class/ClassFile.h"
#include "JVM/Class/ClassFinder.h"
#include "JVM/Core/ErrorOr.h"
#include "JVM/VM/ObjectRepresentation.h"
#include "JVM/string_view"
#include <mutex>
#include <string>
#include <unordered_map>

namespace jvm {
struct Class;
}

using LockType = std::pair<std::condition_variable, std::mutex>;
using LoadedClass = std::pair<LockType, std::unique_ptr<jvm::Class>>;

namespace jvm {

struct Class {
  enum State {
    Unknown = -1,
    Erroneous = 0,
    VerifiedExistence,
    BeingLoaded,
    Loaded,
    Initialized, // <clinit> ran
  };

  State state = Erroneous;
  ClassLocation location;
  std::string_view name;

  std::unordered_map<std::string, uint64_t> staticVars;

  // No initialization is required for statics because they will be zero when
  // they are first aquired. std::unordered_map::operator[] can be a little
  // sketchy, but in this case, it works well. There is of course no check if
  // str is actually a member of this class. This should be ensured elsewhere.
  uint64_t &getStatic(std::string_view str) {
    return staticVars[{str.data(), str.size()}];
  }

  std::unique_ptr<ClassFile> loadedClass;
  // super in [0], interfaces in [1:]
  std::vector<std::reference_wrapper<Class>> superClasses;
  std::recursive_mutex monitor;

  ObjectRepresentation objectRepresentation;

  Class() = default;
};

} // namespace jvm

#endif // JVM_VM_CLASS_H
