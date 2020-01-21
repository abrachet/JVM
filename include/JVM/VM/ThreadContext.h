
#ifndef JVM_VM_THREADCONTEXT_H
#define JVM_VM_THREADCONTEXT_H

#include "JVM/Class/ClassLoader.h"
#include "JVM/VM/Stack.h"
#include <functional>

struct ThreadContext {
  Stack stack;
  std::string loadedClassName;
  const void *pc = nullptr;

  ThreadContext(Stack &&stack) : stack(std::move(stack)) {}

  ClassLoader::LoadedClass &getLoadedClass();
  void callNext();
};

#endif // JVM_VM_THREADCONTEXT_H
