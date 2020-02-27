// Copyright 2020 Alex Brachet
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef JVM_VM_THREADCONTEXT_H
#define JVM_VM_THREADCONTEXT_H

#include "JVM/VM/ClassLoader.h"
#include "JVM/VM/Stack.h"
#include <functional>
#include <vector>

struct Frame {
  const void *returnAddress = nullptr;
  void *frameStart = nullptr;
  std::string_view className;
  uint16_t nameIndex = 0;
  uint16_t typeIndex = 0;
  bool syncronizedMethod = false;

  Frame(std::string_view className, const void *returnAddress = nullptr,
        void *frameStart = nullptr, uint16_t nameIndex = 0,
        uint16_t typeIndex = 0)
      : returnAddress(returnAddress), frameStart(frameStart),
        className(className), nameIndex(nameIndex), typeIndex(typeIndex) {}

  std::string_view getMethodName() const {
    if (!nameIndex)
      return "<unkown method name>";
    auto classOrErr = ClassLoader::getLoadedClass(className);
    assert(classOrErr);
    const auto &cp = classOrErr->second->loadedClass->getConstPool();
    return cp.get<Class::ConstPool::Utf8Info>(nameIndex);
  }

  std::string_view getMethodTypeName() const {
    if (!typeIndex)
      return "<unkown method type name>";
    auto classOrErr = ClassLoader::getLoadedClass(className);
    assert(classOrErr);
    const auto &cp = classOrErr->second->loadedClass->getConstPool();
    return cp.get<Class::ConstPool::Utf8Info>(typeIndex);
  }
};

struct ThreadContext {
  Stack stack;
  std::vector<Frame> frames;
  const void *pc = nullptr;

  // TODO: Need to refactor this out into a static create most likely.
  ThreadContext(Stack &&stack) : stack(std::move(stack)) {}

  LoadedClass &getLoadedClass();
  const ClassFile &getClassFile() {
    return *getLoadedClass().second->loadedClass;
  }

  std::string_view getMethodName() const {
    return currentFrame().getMethodName();
  }

  std::string_view getMethodTypeName() const {
    return currentFrame().getMethodTypeName();
  }

  void pushFrame(Frame &&f) { frames.push_back(f); }
  void pushFrame(std::string_view className, const void *returnAddr = nullptr) {
    pushFrame({className, returnAddr ? returnAddr : pc, stack.sp});
  }

  size_t numFrames() const { return frames.size(); }

  const Frame &currentFrame() const { return frames.back(); }
  Frame &currentFrame() { return frames.back(); }

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
