
#ifndef JVM_VM_THREADCONTEXT_H
#define JVM_VM_THREADCONTEXT_H

#include "JVM/Class/ClassLoader.h"
#include "JVM/VM/Stack.h"
#include <functional>
#include <vector>

struct Frame {
  std::string_view className;
  const void *returnAddress = nullptr;
  const void *frameStart = nullptr;
};

struct ThreadContext {
  Stack stack;
  std::vector<Frame> frames;
  const void *pc = nullptr;

  ThreadContext(Stack &&stack) : stack(std::move(stack)) {}

  ClassLoader::LoadedClass &getLoadedClass();
  const ClassFile &getClassFile() {
    return *getLoadedClass().second.loadedClass;
  }

  void pushFrame(Frame &&f) { frames.push_back(f); }
  void pushFrame(std::string_view className, const void *returnAddr = nullptr) {
    pushFrame({className, returnAddr ? returnAddr : pc, stack.sp});
  }

  Frame popFrame() {
    Frame frame = frames.back();
    frames.pop_back();
    return frame;
  }

  std::string_view getCurrentClassName() { return frames.back().className; }

  void callNext();
};

#endif // JVM_VM_THREADCONTEXT_H
