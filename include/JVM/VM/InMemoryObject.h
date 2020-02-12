
#ifndef JVM_VM_INMEMORYOBJECT_H
#define JVM_VM_INMEMORYOBJECT_H

#include "JVM/VM/ClassLoader.h"
#include "JVM/VM/Stack.h"
#include <mutex>
#include <string_view>

namespace jvm {
constexpr size_t requiredTypeAlignment = Stack::stackEntryBytes * 2;
}

struct alignas(jvm::requiredTypeAlignment) InMemoryObject {
  std::string_view className;
  std::recursive_mutex monitor;

  void *getThisptr() const {
    const char *addr = reinterpret_cast<const char *>(this);
    addr += sizeof(InMemoryObject);
    return const_cast<void *>(reinterpret_cast<const void *>(addr));
  }
};

#endif // JVM_VM_INMEMORYOBJECT_H