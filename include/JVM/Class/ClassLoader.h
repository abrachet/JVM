
#ifndef JVM_CLASS_CLASSLOADER_H
#define JVM_CLASS_CLASSLOADER_H

#include "JVM/Class/ClassFile.h"
#include "JVM/Class/ClassFinder.h"
#include "JVM/Core/ErrorOr.h"
#include "JVM/string_view"
#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class ClassLoader {
  using LockType = std::pair<std::condition_variable, std::mutex>;

public:
  struct LoadedClass;

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
    std::unique_ptr<ClassFile> loadedClass;
    // super in [0], interfaces in [1:]
    std::vector<std::reference_wrapper<LoadedClass>> superClasses;
  };

  inline static std::vector<std::string> classPath = {"."};

  struct LoadedClass : public std::pair<LockType, Class> {};
  static ErrorOr<LoadedClass &> loadClass(const std::string_view fullClassName);

  static Class::State findClassState(const std::string_view fullClassName) {
    std::string str(fullClassName.data(), fullClassName.size());
    std::scoped_lock l(loadedClassesMutex);
    auto it = loadedClasses.find(str);
    if (it != loadedClasses.end())
      return it->second.second.state;
    return Class::State::Unknown;
  }

  static int numLoadedClasses() {
    std::scoped_lock l(loadedClassesMutex);
    return loadedClasses.size();
  }

private:
  inline static std::mutex loadedClassesMutex;
  inline static std::unordered_map<std::string, LoadedClass> loadedClasses;

  static std::string loadSuperClasses(Class &);
};

#endif // JVM_CLASS_CLASSLOADER_H
