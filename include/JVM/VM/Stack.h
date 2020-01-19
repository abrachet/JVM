
#ifndef JVM_VM_STACK_H
#define JVM_VM_STACK_H

#include "JVM/Core/ErrorOr.h"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <system_error>
#include <unistd.h>

struct Stack {
  // TODO: The error callback should be handled by mprotecting a guard page.
  using CallbackType = std::function<void(Stack &)>;
  CallbackType stackOverflowCallback = nullptr;
  void *stack = nullptr;
  size_t size;
  void *sp;

  static ErrorOr<Stack> createStack(size_t size = 0x200000,
                                    CallbackType cb = nullptr);

  template <int entries> void push(uint64_t toPush);
  template <> void push<1>(uint64_t toPush) { push<uint32_t>(toPush); }
  template <> void push<2>(uint64_t toPush) { push<uint64_t>(toPush); }

  template <int entries> uint64_t pop();
  template <> uint64_t pop<1>() { return pop<uint32_t>(); }
  template <> uint64_t pop<2>() { return pop<uint64_t>(); }

  Stack(const Stack &) = delete;
  Stack(Stack &&other)
      : stackOverflowCallback(other.stackOverflowCallback), stack(other.stack),
        size(other.size), sp(other.sp) {
    other.stack = nullptr;
  }

  ~Stack();

private:
  Stack() = default;

  // TODO (During addition of JIT): Hand code this to keep all variables in
  // registers so that it never messes with the programs stack.
  template <typename T> void push(T toPush) {
    if (reinterpret_cast<T *>(sp) - 1 < reinterpret_cast<T *>(stack))
      stackOverflowCallback(*this);
    T *top = reinterpret_cast<T *>(sp) - 1;
    *top = static_cast<T>(toPush);
    sp = reinterpret_cast<T *>(sp) - 1;
  }

  template <typename T> uint64_t pop() {
    // TODO: assert for stack underflow.
    T *top = reinterpret_cast<T *>(sp);
    uint64_t ret = *top;
    sp = top + 1;
    return ret;
  }
};

#endif // JVM_VM_STACK_H
