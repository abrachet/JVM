
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

#endif // JVM_CORE_BIGENDIANBYTEREADER_H
