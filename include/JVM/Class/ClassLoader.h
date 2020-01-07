
#include "JVM/Class/ClassFile.h"
#include "JVM/Class/ClassFinder.h"
#include "JVM/string_view"
#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

class ClassLoader {
  using LockType = std::pair<std::condition_variable, std::mutex>;

  class Class {
    enum State {
      Erroneous = 0,
      VerifiedExistence,
      BeingLoaded,
      Loaded,
      Initialized, // <clinit> ran
    };

    std::string fullClassName;

    State state;
    ClassLocation location; // Only meanignful when state is VerifiedExistence.
    std::unique_ptr<ClassFile> loadedClass;
  };

public:
  static std::vector<std::string> classPath;

  using LoadedClass = std::unique_ptr<std::pair<LockType, Class>>;
  using LoadedClassOrErr = std::pair<LoadedClass, std::string>;
  static LoadedClassOrErr loadClass(const std::string_view fullClassName);
};
