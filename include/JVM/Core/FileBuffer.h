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

#ifndef JVM_CORE_FILEBUFFER_H
#define JVM_CORE_FILEBUFFER_H

#include <cstddef>
#include <memory>
#include <string>

class FileBuffer {
public:
  virtual ~FileBuffer() = default;

  static std::unique_ptr<FileBuffer> create(const std::string &filename);

  using FDType = int;

  virtual operator const char *() const = 0;
  virtual operator char *() = 0;
  virtual size_t size() const = 0;
  virtual std::string getNameIfAvailable() const { return ""; }
  virtual bool writeToFile(FDType) const { return false; }
};

class MMappedFileBuffer : public FileBuffer {
  friend FileBuffer;

  int fd = -1;
  size_t len;
  char *buf = nullptr;
  std::string filename = {};

  MMappedFileBuffer(int fd, size_t len, char *buf, std::string filename = {})
      : fd(fd), len(len), buf(buf), filename(filename) {}

public:
  ~MMappedFileBuffer() override;

  operator const char *() const override { return buf; }

  operator char *() override { return buf; }

  size_t size() const override { return len; }

  std::string getNameIfAvailable() const override { return filename; }
  bool writeToFile(FileBuffer::FDType) const override;
};

#endif // JVM_CORE_FILEBUFFER_H
