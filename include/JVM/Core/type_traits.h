
#ifndef JVM_CORE_TYPE_TRAITS_H
#define JVM_CORE_TYPE_TRAITS_H

#if __cplusplus != 201803L

namespace std {

template <typename T> struct type_identity { using type = T; };
template <typename T> using type_identity_t = typename type_identity<T>::type;

} // namespace std

#endif

#endif // JVM_CORE_TYPE_TRAITS_H
