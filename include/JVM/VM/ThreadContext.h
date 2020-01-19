
#ifndef JVM_VM_THREADCONTEXT_H
#define JVM_VM_THREADCONTEXT_H

#include "JVM/VM/Stack.h"

struct ThreadContext {
  Stack stack;
  const void *pc = nullptr;

  ThreadContext(Stack &&stack) : stack(std::move(stack)) {}

  void callNext();
};

#endif // JVM_VM_THREADCONTEXT_H
