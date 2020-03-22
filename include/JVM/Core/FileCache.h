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

#ifndef JVM_CORE_FILECACHE_H
#define JVM_CORE_FILECACHE_H

#include "JVM/Core/ErrorOr.h"
#include "JVM/Core/FileBuffer.h"
#include "JVM/string_view"
#include <chrono>
#include <string>
#include <utility>
#include <vector>

class FileCache {
  using time_point = std::chrono::system_clock::time_point;
  using File = std::pair<std::string, time_point>;

  std::string initDir;
  unsigned maxSize = 100;

  mutable std::vector<File> cachedFiles;
  ErrorOr<const std::vector<File> &> walkDirectory() const;

  void removeOldest() const;

  std::string getFilename(std::string_view filename) const;
  static bool alwaysReject(std::string_view filename);

  static void mkdir(std::string_view dir);

public:
  FileCache(std::string_view initDir, unsigned maxSize = 100);

  std::string getFileIfInCache(std::string_view filename) const;
  void cacheFile(const FileBuffer &file) const;

  std::string_view getDir() const { return initDir; }
};

#endif // JVM_CORE_FILECACHE_H
