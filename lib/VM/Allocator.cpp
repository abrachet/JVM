
#include "JVM/VM/Allocator.h"
#include "JVM/VM/Class.h"
#include "JVM/VM/InMemoryObject.h"
#include <cstdlib>
#include <cstring>

InMemoryObject *jvm::allocate(const jvm::Class &clss) {
  size_t objectSize = clss.objectRepresentation.getObjectSize();
  void *const mem = aligned_alloc(jvm::requiredTypeAlignment,
                                  sizeof(InMemoryObject) + objectSize);
  new (mem) InMemoryObject{clss};
  std::memset(reinterpret_cast<char *>(mem) + sizeof(InMemoryObject), 0,
              objectSize);
  assert(!(reinterpret_cast<uintptr_t>(reinterpret_cast<char *>(mem) +
                                       sizeof(InMemoryObject)) %
           jvm::requiredTypeAlignment));
  return reinterpret_cast<InMemoryObject *>(mem);
}

void jvm::deallocate(InMemoryObject *ptr) { std::free(ptr); }
