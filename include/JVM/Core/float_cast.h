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
