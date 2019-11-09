
#include "JVM/Core/FileBuffer.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

FileBuffer::~FileBuffer() {
    if (fd != -1 && buf != MAP_FAILED)
        munmap(buf, len + 1);
}

std::unique_ptr<FileBuffer> FileBuffer::create(std::string_view filename) {
  int fd = open(filename.data(), O_RDWR);
  if (fd == -1)
    return nullptr;
  struct stat sb;
  if (fstat(fd, &sb))
    return nullptr;
  void* mapping =
      mmap(nullptr, sb.st_size + 1, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (mapping == MAP_FAILED)
    return nullptr;
  madvise(mapping, sb.st_size + 1, MADV_SEQUENTIAL);
  // Can't use make_unique with private ctor.
  return std::unique_ptr<FileBuffer>(new FileBuffer(fd, sb.st_size,
                                      static_cast<char*>(mapping)));
}
