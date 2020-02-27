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

#ifndef JVM_CLASS_CLASSLOADER_H
#define JVM_CLASS_CLASSLOADER_H

#include "JVM/Class/ClassFile.h"
#include "JVM/Class/ClassFinder.h"
#include "JVM/Core/ErrorOr.h"
#include "JVM/VM/Class.h"
#include "JVM/VM/ObjectRepresentation.h"
#include "JVM/string_view"
#include <condition_variable>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class ClassLoader {
  using Class = jvm::Class;

public:
  inline static std::vector<std::string> classPath = {"."};

  static ErrorOr<LoadedClass &> loadClass(const std::string_view fullClassName);
  static ErrorOr<LoadedClass &>
  getLoadedClass(const std::string_view fullClassName);

  static Class::State findClassState(const std::string_view fullClassName) {
    std::string str(fullClassName.data(), fullClassName.size());
    std::shared_lock l(loadedClassesMutex);
    auto it = loadedClasses.find(str);
    if (it != loadedClasses.end())
      return it->second.second->state;
    return Class::State::Unknown;
  }

  static int numLoadedClasses() {
    std::shared_lock l(loadedClassesMutex);
    return loadedClasses.size();
  }

private:
  inline static std::shared_mutex loadedClassesMutex;
  inline static std::unordered_map<std::string, LoadedClass> loadedClasses;

  static std::string loadSuperClasses(Class &);
};

#endif // JVM_CLASS_CLASSLOADER_H
