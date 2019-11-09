
#ifndef JVM_CORE_FILEREADER_H
#define JVM_CORE_FILEREADER_H

#include "FileBuffer.h"
#include <cstddef>
#include <string_view>
#include <type_traits>
#include <memory>

class FileReader {
  std::unique_ptr<FileBuffer> buf;
  size_t pos = 0;

  template <typename T>
  typename std::enable_if<std::is_pointer<T>::value, void>::type unsafeRead(T& t) {
    const T ptr = reinterpret_cast<const T>(*buf + pos);
    pos += sizeof(*ptr);
    t = ptr;
  }

  template <typename T>
  typename std::enable_if<!std::is_pointer<T>::value, void>::type unsafeRead(T& t) {
    const T* ptr = reinterpret_cast<const T*>(*buf + pos);
    pos += sizeof(T);
    t = *ptr;
  }

  template <typename T>
  constexpr size_t getTemplateSize() {
    if constexpr (std::is_pointer<T>::value)
      return sizeof(typename std::remove_pointer<T>::type);
    else
      return sizeof(T);
  }

public:
  static std::unique_ptr<FileReader> create(std::string_view filename);

  // Using a T that is a pointer type will set t to a proper pointer in the
  // file but it's lifetime is that of the file.
  template <typename T> bool read(T& t) {
    if (!buf || getTemplateSize<T>() + pos > buf->size())
      return false;
    unsafeRead(t);
    return true;
  }

  const char *data() const {
    return *buf;
  }

  size_t getPos() const {
    return pos;
  }

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

#endif // JVM_CORE_FILEREADER_H