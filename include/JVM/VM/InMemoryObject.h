
#ifndef JVM_VM_INMEMORYOBJECT_H
#define JVM_VM_INMEMORYOBJECT_H

#include "JVM/VM/Class.h"
#include "JVM/VM/ClassLoader.h"
#include "JVM/VM/Stack.h"
#include "JVM/string_view"
#include <mutex>

namespace jvm {
constexpr size_t requiredTypeAlignment = Stack::stackEntryBytes * 2;
}

struct alignas(jvm::requiredTypeAlignment) InMemoryObject {
  const jvm::Class &clss;
  std::recursive_mutex monitor;

  void *getThisptr() const {
    const char *addr = reinterpret_cast<const char *>(this);
    addr += sizeof(InMemoryObject);
    return const_cast<void *>(reinterpret_cast<const void *>(addr));
  }

  std::string_view getName() const { return clss.name; }
};

#endif // JVM_VM_INMEMORYOBJECT_H