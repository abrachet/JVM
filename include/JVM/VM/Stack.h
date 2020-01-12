
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

  template <int entries> inline void push(uint64_t toPush);
  template <> inline void push<1>(uint64_t toPush) { push<uint32_t>(toPush); }
  template <> inline void push<2>(uint64_t toPush) { push<uint64_t>(toPush); }

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
  template <typename T> inline void push(T toPush) {
    if (reinterpret_cast<T *>(sp) - 1 < reinterpret_cast<T *>(stack))
      stackOverflowCallback(*this);
    T *top = reinterpret_cast<T *>(sp) - 1;
    *top = static_cast<T>(toPush);
    sp = reinterpret_cast<T *>(sp) - 1;
  }
};
