
#ifndef JVM_CORE_ALGORITHM_H
#define JVM_CORE_ALGORITHM_H

#include <algorithm>
#include <type_traits>

// TOOD: This code will be a lot less ugly when concepts land.

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

namespace jvm {

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

} // namespace jvm

#endif // JVM_CORE_ALGORITHM_H
