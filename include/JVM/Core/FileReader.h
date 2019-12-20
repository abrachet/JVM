
#ifndef JVM_CORE_FILEREADER_H
#define JVM_CORE_FILEREADER_H

#include <cassert>
#include <climits>
#include <cstddef>
#include <memory>
#include <string>
#include <type_traits>
#include "FileBuffer.h"

template <bool swapEndianOfReadsDefault = false>
class FileReader {
  std::unique_ptr<FileBuffer> buf;
  size_t pos = 0;

  template <typename T>
  T swap(T u) {
    static_assert(CHAR_BIT == 8);

    union {
      T u;
      unsigned char u8[sizeof(T)];
    } source, dest;

    source.u = u;
    for (size_t i = 0; i < sizeof(T); i++)
      dest.u8[i] = source.u8[sizeof(T) - i - 1];

    return dest.u;
  }

  template <typename T, bool swapEndian>
  typename std::enable_if<std::is_pointer<T>::value, void>::type unsafeRead(
      T& t) {
    static_assert(!swapEndian,
                  "Can't swap endianess when using read with a pointer");
    const T ptr = reinterpret_cast<const T>(*buf + pos);
    pos += sizeof(*ptr);
    t = ptr;
  }

  template <typename T, bool swapEndian>
  typename std::enable_if<!std::is_pointer<T>::value, void>::type unsafeRead(
      T& t) {
    const T* ptr = reinterpret_cast<const T*>(*buf + pos);
    pos += sizeof(T);
    t = swapEndian ? swap(*ptr) : *ptr;
  }

  template <typename T>
  constexpr size_t getTemplateSize() {
    if constexpr (std::is_pointer<T>::value)
      return sizeof(typename std::remove_pointer<T>::type);
    else
      return sizeof(T);
  }

 public:
  static std::unique_ptr<FileReader> create(const std::string& filename) {
    auto reader = std::make_unique<FileReader>();
    reader->buf = FileBuffer::create(filename);
    return reader;
  }

  // Using a T that is a pointer type will set t to a proper pointer in the
  // file but it's lifetime is that of the file.
  template <typename T, bool swapEndian = swapEndianOfReadsDefault>
  bool read(T& t) {
    if (!buf || getTemplateSize<T>() + pos > buf->size())
      return false;
    unsafeRead<T, swapEndian>(t);
    return true;
  }

  const char* data() const { return *buf; }

  size_t getPos() const { return pos; }

  void seek(size_t newPos) {
    assert(!buf || newPos <= buf->size());
    pos = newPos;
  }

  std::unique_ptr<FileBuffer> takeFileBuffer() {
    auto ret = std::move(buf);
    buf = nullptr;
    return ret;
  }
};

#endif  // JVM_CORE_FILEREADER_H
