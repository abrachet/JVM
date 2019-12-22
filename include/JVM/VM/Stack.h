
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <sys/mman.h>
#include <system_error>
#include <unistd.h>

#ifndef MAP_UNINITIALIZED
#define MAP_UNINITIALIZED 0
#endif

struct Stack {
  using CallbackType = std::function<void(Stack &)>;
  CallbackType stackOverflowCallback = nullptr;
  void *stack = nullptr;
  size_t size;
  void *sp;

  static std::error_code createStack(Stack &s, size_t size = 0x200000,
                                     CallbackType cb = nullptr) {
    assert(!(size & (getpagesize() - 1)) && "size not page aligned");
    s.size = size;
    s.stackOverflowCallback = cb;
    s.stack = mmap(nullptr, size, PROT_READ | PROT_WRITE,
                   MAP_ANONYMOUS | MAP_SHARED | MAP_UNINITIALIZED, 0, 0);
    if (s.stack == MAP_FAILED) {
      int savedErrno = errno;
      errno = 0;
      return {savedErrno, std::system_category()};
    }

    s.sp = reinterpret_cast<char *>(s.stack) + size;
    return {};
  }


  template <int entries> inline void push(uint64_t toPush);

  template <> inline void push<1>(uint64_t toPush) { push<uint32_t>(toPush); }

  template <> inline void push<2>(uint64_t toPush) { push<uint64_t>(toPush); }

  Stack() = default;
  Stack(const Stack &) = delete;
  Stack(Stack &&) = default;

  ~Stack() {
    int saveErr = errno;
    if (stack)
      munmap(stack, size);
    assert(errno == saveErr);
    stack = nullptr;
  }

private:
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
