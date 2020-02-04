
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

  // TODO: Need to refactor this out into a static create most likely.
  ThreadContext(Stack &&stack) : stack(std::move(stack)) {}

  ClassLoader::LoadedClass &getLoadedClass();
  const ClassFile &getClassFile() {
    return *getLoadedClass().second.loadedClass;
  }

  void pushFrame(Frame &&f) { frames.push_back(f); }
  void pushFrame(std::string_view className, const void *returnAddr = nullptr) {
    pushFrame({className, returnAddr ? returnAddr : pc, stack.sp});
  }

  const Frame &currentFrame() const { return frames.back(); }

  Frame popFrame() {
    Frame frame = frames.back();
    frames.pop_back();
    pc = frame.returnAddress;
    return frame;
  }

  template <size_t Size> uint64_t loadFromLocal(int index) {
    static_assert(Stack::validEntrySize(Size), "Invalid stack entry size");
    const uint32_t *stack =
        reinterpret_cast<const uint32_t *>(currentFrame().frameStart);
    stack -= index;
    stack -= Size;
    return *reinterpret_cast<const Stack::EntryType<Size> *>(stack);
  }

  std::string_view getCurrentClassName() { return frames.back().className; }

  void callNext();
};

#endif // JVM_VM_THREADCONTEXT_H
