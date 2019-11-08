
#ifndef JVM_CORE_FILEBUFFER_H
#define JVM_CORE_FILEBUFFER_H

#include <string_view>
#include <cstddef>

class FileBuffer {
    int fd = -1;
    size_t size;
    char *buf = nullptr;

    FileBuffer(int fd, size_t size, char* buf) : fd(fd), size(size), buf(buf) {}

public:
    ~FileBuffer();

    static std::unique_ptr<FileBuffer> create(std::string_view filename);

    operator const char*() const { return buf; }
    operator char*() { return buf; }
};

#endif // JVM_CORE_FILEBUFFER_H
