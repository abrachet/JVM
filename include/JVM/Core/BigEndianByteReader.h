
#ifndef JVM_CORE_BIGENDIANBYTEREADER_H
#define JVM_CORE_BIGENDIANBYTEREADER_H

#ifndef __BYTE_ORDER
#define __BYTE_ORDER 1 // Assume little endian.
#define __LITTLE_ENDIAN 1
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

#endif // JVM_CORE_BIGENDIANBYTEREADER_H
