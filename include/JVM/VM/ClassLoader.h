
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
