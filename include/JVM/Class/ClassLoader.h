
#ifndef JVM_CLASS_CLASSLOADER_H
#define JVM_CLASS_CLASSLOADER_H

#include "JVM/Class/ClassFile.h"
#include "JVM/Class/ClassFinder.h"
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
  struct Class {
    enum State {
      Erroneous = 0,
      VerifiedExistence,
      BeingLoaded,
      Loaded,
      Initialized, // <clinit> ran
    };

    State state = Erroneous;
    ClassLocation location;
    std::unique_ptr<ClassFile> loadedClass;
  };

  // shared_ptr end's up being much more ergonomic than unique_ptr, or returning
  // a reference to a 'std::pair<LockType, Class>'.
  using LoadedClass = std::pair<LockType, Class>;

  static std::vector<std::string> classPath;

  using LoadedClassOrErr = std::pair<LoadedClass &, std::string>;
  static LoadedClassOrErr loadClass(const std::string_view fullClassName);

private:
  static std::unordered_map<std::string, LoadedClass> loadedClasses;
};

#endif // JVM_CLASS_CLASSLOADER_H
