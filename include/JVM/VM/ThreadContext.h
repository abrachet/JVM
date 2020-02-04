
#ifndef JVM_VM_THREADCONTEXT_H
#define JVM_VM_THREADCONTEXT_H

#include "JVM/Class/ClassLoader.h"
#include "JVM/VM/Stack.h"
#include <functional>
#include <vector>

struct Frame {
  std::string_view className;
  const void *returnAddress = nullptr;
  void *frameStart = nullptr;
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

private:
  template <size_t Width> void *getAddressOfLocal(int index) {
    static_assert(Stack::validEntrySize(Width), "Invalid stack entry size");
    uint32_t *stack = reinterpret_cast<uint32_t *>(currentFrame().frameStart);
    stack -= index;
    stack -= Width;
    return reinterpret_cast<void *>(stack);
  }

public:
  template <size_t Width> uint64_t loadFromLocal(int index) {
    static_assert(Stack::validEntrySize(Width), "Invalid stack entry size");
    const uint32_t *stack =
        reinterpret_cast<const uint32_t *>(getAddressOfLocal<Width>(index));
    return *reinterpret_cast<const Stack::EntryType<Width> *>(stack);
  }

  template <size_t Width> void storeInLocal(int index, uint64_t toStore) {
    static_assert(Stack::validEntrySize(Width), "Invalid stack entry size");
    auto *addr = reinterpret_cast<Stack::EntryType<Width> *>(
        getAddressOfLocal<Width>(index));
    *addr = toStore;
  }

  std::string_view getCurrentClassName() { return frames.back().className; }

  void callNext();
};

#endif // JVM_VM_THREADCONTEXT_H
