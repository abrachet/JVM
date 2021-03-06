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

#ifndef JVM_CORE_ALGORITHM_H
#define JVM_CORE_ALGORITHM_H

#include <algorithm>
#include <numeric>
#include <type_traits>

// TOOD: This code will be a lot less ugly when concepts land.

namespace algo_detail {

template <typename T> class has_begin_impl {
  using yes = char[1];
  using no = char[2];

  template <typename Other>
  static yes &test(Other *o, decltype(o->begin()) * = nullptr);

  template <typename> static no &test(...);

public:
  static constexpr bool value = sizeof(test<T>(nullptr)) == sizeof(yes);
};

template <typename T>
struct has_begin : has_begin_impl<typename std::remove_reference<T>::type> {};

} // namespace algo_detail

namespace jvm {

using algo_detail::has_begin;

template <typename Iterable, typename OutputIt, typename UnaryOperation>
OutputIt transform(Iterable &&range, OutputIt d_first, UnaryOperation unary_op,
                   std::enable_if_t<has_begin<Iterable>::value> * = nullptr) {
  return std::transform(std::begin(range), std::end(range), d_first, unary_op);
}

template <typename Iterable, typename T>
void replace(Iterable &&range, const T &old_value, const T &new_value,
             std::enable_if_t<has_begin<Iterable>::value> * = nullptr) {
  std::replace(std::begin(range), std::end(range), old_value, new_value);
}

template <typename Iterable, typename UnaryPredicate>
auto find_if(Iterable &&range, UnaryPredicate p,
             std::enable_if_t<has_begin<Iterable>::value> * = nullptr) {
  return std::find_if(std::begin(range), std::end(range), p);
}

template <typename Iterable, typename T, typename BinaryFunction>
T accumulate(Iterable &&range, T init, BinaryFunction op) {
  static_assert(has_begin<Iterable>::value);
  return std::accumulate(std::begin(range), std::end(range), init, op);
}

template <typename Iterable, typename Compare>
auto max_element(Iterable &&range, Compare comp) {
  return std::max_element(std::begin(range), std::end(range), comp);
}

} // namespace jvm

#endif // JVM_CORE_ALGORITHM_H
