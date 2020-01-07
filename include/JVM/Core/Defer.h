
#ifndef JVM_CORE_DEFER_H
#define JVM_CORE_DEFER_H

#include <type_traits>
#include <utility>

template <typename Callable> struct ScopedExit {
  Callable c;

  ScopedExit(Callable &&c) : c(c) {}
  ~ScopedExit() { c(); }
};

template <typename Callable>[[nodiscard]] auto defer(Callable &&callable) {
  return ScopedExit<typename std::decay<Callable>::type>(
      std::forward<Callable>(callable));
}

#endif // JVM_CORE_DEFER_H
