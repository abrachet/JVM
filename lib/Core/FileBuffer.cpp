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

#include "JVM/Core/FileBuffer.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

MMappedFileBuffer::~MMappedFileBuffer() {
  if (fd != -1 && buf != MAP_FAILED)
    munmap(buf, len + 1);
}

std::unique_ptr<FileBuffer> FileBuffer::create(const std::string& filename) {
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
  return std::unique_ptr<FileBuffer>(
      new MMappedFileBuffer(fd, sb.st_size, static_cast<char*>(mapping)));
}
