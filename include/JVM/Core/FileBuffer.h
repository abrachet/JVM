
#ifndef JVM_CORE_FILEBUFFER_H
#define JVM_CORE_FILEBUFFER_H

#include <cstddef>
#include <memory>
#include <string>

class FileBuffer {
public:
  virtual ~FileBuffer() = default;

  static std::unique_ptr<FileBuffer> create(const std::string &filename);

  virtual operator const char *() const = 0;
  virtual operator char *() = 0;
  virtual size_t size() const = 0;
};

class MMappedFileBuffer : public FileBuffer {
  friend FileBuffer;

  int fd = -1;
  size_t len;
  char *buf = nullptr;

  MMappedFileBuffer(int fd, size_t len, char *buf)
      : fd(fd), len(len), buf(buf) {}

public:
  ~MMappedFileBuffer() override;

  operator const char *() const override { return buf; }

  operator char *() override { return buf; }

  size_t size() const override { return len; }
};

#endif // JVM_CORE_FILEBUFFER_H
