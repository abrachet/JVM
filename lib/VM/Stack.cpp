
#include "JVM/VM/Stack.h"
#include "JVM/Core/Defer.h"

#include <sys/mman.h>

#ifndef MAP_UNINITIALIZED
#define MAP_UNINITIALIZED 0
#endif

ErrorOr<Stack> Stack::createStack(size_t size, CallbackType cb) {
  assert(!(size & (getpagesize() - 1)) && "size not page aligned");
  auto _ = defer([] { errno = 0; });
  Stack s;
  s.size = size;
  s.stackOverflowCallback = cb;
  s.stack = mmap(nullptr, size + getpagesize(), PROT_READ | PROT_WRITE,
                 MAP_ANONYMOUS | MAP_SHARED | MAP_UNINITIALIZED, 0, 0);
  if (s.stack == MAP_FAILED) {
    int savedErrno = errno;
    errno = 0;
    return std::error_code(savedErrno, std::system_category()).message();
  }

  s.sp = reinterpret_cast<char *>(s.stack) + size;
  return s;
}

Stack::~Stack() {
  int saveErr = errno;
  if (stack)
    munmap(stack, size);
  assert(errno == saveErr);
  stack = nullptr;
  errno = 0;
}