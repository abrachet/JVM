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

#ifndef JVM_CORE_BIGENDIANBYTEREADER_H
#define JVM_CORE_BIGENDIANBYTEREADER_H

#include <cstdint>

#ifndef __BYTE_ORDER
#define __BYTE_ORDER 1234 // Assume little endian.
#define __LITTLE_ENDIAN 1234
#endif

template <typename T> void readFromPointer(T &t, const uint8_t *&ptr) {
  t = *reinterpret_cast<const T *>(ptr);
#if __BYTE_ORDER == __LITTLE_ENDIAN
  if constexpr (sizeof(T) == 8)
    t = __builtin_bswap64(t);
  if constexpr (sizeof(T) == 4)
    t = __builtin_bswap32(t);
  if constexpr (sizeof(T) == 2)
    t = __builtin_bswap16(t);
#endif
  ptr += sizeof(T);
}

template <typename T> T readFromPointer(const void *&ptr) {
  T t;
  const uint8_t *ptr8 = reinterpret_cast<const uint8_t *>(ptr);
  readFromPointer(t, ptr8);
  ptr = ptr8;
  return t;
}

template <size_t Size> struct size_to_type_impl {};
template <> struct size_to_type_impl<1> { using type = uint8_t; };
template <> struct size_to_type_impl<2> { using type = uint16_t; };
template <> struct size_to_type_impl<3> { using type = uint32_t; };
template <> struct size_to_type_impl<4> { using type = uint64_t; };

template <size_t Size>
using size_to_type = typename size_to_type_impl<Size>::type;

template <size_t Size> size_to_type<Size> readFromPointer(const void *&ptr) {
  return readFromPointer<size_to_type<Size>>(ptr);
}

#endif // JVM_CORE_BIGENDIANBYTEREADER_H
