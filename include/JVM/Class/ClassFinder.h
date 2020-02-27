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

#ifndef JVM_CLASS_CLASSFINDER_H
#define JVM_CLASS_CLASSFINDER_H

#include "JVM/Core/FileBuffer.h"
#include "JVM/string_view"
#include <cassert>
#include <functional>
#include <string>
#include <vector>

struct ClassLocation {
  enum LocationType { NoExist, File, InJar };

  std::string className;
  std::string path;
  LocationType type = NoExist;

  ClassLocation() = default;
  ClassLocation(std::string className, std::string path,
                LocationType type = File)
      : className(className), path(path), type(type) {}
};

class ZipFileBuffer : public FileBuffer {
  size_t fileSize;
  char *mappedFile;

  ZipFileBuffer() {}

public:
  static std::unique_ptr<FileBuffer> create(std::string_view zipFile,
                                            std::string_view entry);
  ~ZipFileBuffer() override;

  operator const char *() const override { return mappedFile; }
  operator char *() override { return mappedFile; }
  size_t size() const override { return fileSize; }
};

std::string findRTJar(std::string &path);

std::string registerFromJar(const std::string &path,
                            std::function<void(std::string)> registerFn);

ClassLocation findClassLocation(std::string className,
                                const std::vector<std::string> &classPath);

#endif // JVM_CLASS_CLASSFINDER_H
