
#ifndef JVM_CORE_FILEBUFFER_H
#define JVM_CORE_FILEBUFFER_H

#include <cstddef>
#include <memory>
#include <string>

class FileBuffer {
  int fd = -1;
  size_t len;
  char *buf = nullptr;

  FileBuffer(int fd, size_t len, char *buf) : fd(fd), len(len), buf(buf) {}

public:
  ~FileBuffer();

  static std::unique_ptr<FileBuffer> create(const std::string& filename);

  operator const char *() const { return buf; }
  operator char *() { return buf; }
  size_t size() const { return len; }
};

#endif // JVM_CORE_FILEBUFFER_H
