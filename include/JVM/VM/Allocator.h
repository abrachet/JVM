
#ifndef JVM_VM_ALLOCATOR_H
#define JVM_VM_ALLOCATOR_H

#include "JVM/VM/ClassLoader.h"
#include "JVM/VM/InMemoryObject.h"

namespace jvm {

InMemoryObject *allocate(const ClassLoader::Class &);

void deallocate(InMemoryObject *);

} // namespace jvm

#endif // JVM_VM_ALLOCATOR_H