
#ifndef JVM_VM_CLASS_H
#define JVM_VM_CLASS_H

#include "JVM/Class/ClassFile.h"
#include "JVM/Class/ClassFinder.h"
#include "JVM/Core/ErrorOr.h"
#include "JVM/VM/ObjectRepresentation.h"
#include "JVM/string_view"
#include <mutex>

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

  std::unique_ptr<ClassFile> loadedClass;
  // super in [0], interfaces in [1:]
  std::vector<std::reference_wrapper<Class>> superClasses;
  std::recursive_mutex monitor;

  ObjectRepresentation objectRepresentation;

  Class() = default;
};

} // namespace jvm

#endif // JVM_VM_CLASS_H
