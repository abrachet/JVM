
#ifndef JVM_CORE_FLOATCAST_H
#define JVM_CORE_FLOATCAST_H

#include <type_traits>

namespace internal {
template <typename A, typename B> constexpr static inline A cast(B b) {
  B a = b;
  return *reinterpret_cast<A *>(&a);
}
} // namespace internal

template <typename IntT, typename FloatT,
          std::enable_if_t<std::is_integral<IntT>::value, int> = 0,
          std::enable_if_t<std::is_floating_point<FloatT>::value, int> = 0>
constexpr static inline IntT float_cast(FloatT f) {
  return internal::cast<IntT>(f);
}

template <typename FloatT, typename IntT,
          std::enable_if_t<std::is_integral<IntT>::value, int> = 0,
          std::enable_if_t<std::is_floating_point<FloatT>::value, int> = 0>
constexpr static inline FloatT float_cast(IntT i) {
  return internal::cast<FloatT>(i);
}

#endif // JVM_CORE_FLOATCAST_H
