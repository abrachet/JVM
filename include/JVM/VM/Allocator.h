
#ifndef JVM_VM_ALLOCATOR_H
#define JVM_VM_ALLOCATOR_H

#include "JVM/VM/Class.h"
#include "JVM/VM/ClassLoader.h"
#include "JVM/VM/InMemoryObject.h"

namespace jvm {

InMemoryObject *allocate(const jvm::Class &);

void deallocate(InMemoryObject *);

} // namespace jvm

#endif // JVM_VM_ALLOCATOR_H