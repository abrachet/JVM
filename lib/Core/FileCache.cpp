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

#include "JVM/Core/FileCache.h"
#include "JVM/Core/Defer.h"
#include "JVM/Core/algorithm.h"
#include <algorithm>
#include <dirent.h>
#include <fcntl.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

ErrorOr<const std::vector<FileCache::File> &> FileCache::walkDirectory() const {
  using namespace std::chrono;

  DIR *dir;
  if (!(dir = opendir(initDir.c_str())))
    return std::string("FileCache: couldn't open directory");
  auto _ = defer([dir] { closedir(dir); });

  cachedFiles.clear();

  for (struct dirent *dirent; (dirent = readdir(dir));) {
    if (dirent->d_type == DT_DIR)
      return std::string("FileCache: unexpected directory in cache");
    struct stat sb;
    if (stat(dirent->d_name, &sb) == -1)
      return std::string("FileCache: stat(2) failure");
    cachedFiles.emplace_back(initDir + dirent->d_name,
                             system_clock::from_time_t(sb.st_atime));
  }

  return cachedFiles;
}

void FileCache::removeOldest() const {
  if (cachedFiles.empty())
    return;

  auto it = jvm::max_element(cachedFiles, [](const File &a, const File &b) {
    return a.second < b.second;
  });
  assert(it != cachedFiles.end());

  ::unlink(it->first.c_str());
  // Notably, there is no invalidation here, the array still represents the
  // contents of the directory.
  cachedFiles.erase(it);
}

static bool strEndsWith(const std::string &str, char c) {
  return str.data()[str.size() - 1] == c;
}

// Directories are non trivial to walk and create, '/' gets replaced by
// "._~_." which is an unusual enough sequence that it would never actually
// exist. Moreover it is impossible for a Java class file to have this sequence
// so it is always safe.
std::string FileCache::getFilename(std::string_view filename) const {
  constexpr std::string_view replace = "._~_.";
  assert(strEndsWith(initDir, '/'));
  std::string ret(filename.data(), filename.size());
  for (size_t pos = 0; (pos = ret.find('/', pos)) != std::string::npos;)
    ret.replace(pos, 1, replace);
  return initDir + ret;
}

// Although this pattern should never be in the files we check to make sure we
// don't overwrite a file with a filename which actually has "._~_.".
bool FileCache::alwaysReject(std::string_view filename) {
  return filename.find("._~_.") != filename.npos;
}

void FileCache::mkdir(std::string_view dir) {
  size_t last = dir.rfind('/');
  if (last != std::string_view::npos)
    mkdir({dir.begin(), last});

  std::string str(dir.data(), dir.size());
  if (!::access(str.c_str(), F_OK))
    return;
  int got = ::mkdir(str.c_str(), 0700);

  assert(!got && "mkdir failed");
  (void)got;
}

FileCache::FileCache(std::string_view dir, unsigned size)
    : initDir(dir), maxSize(size) {
  mkdir(initDir);
  if (!strEndsWith(initDir, '/'))
    initDir += '/';
  walkDirectory();
}

#include <iostream>
std::string FileCache::getFileIfInCache(std::string_view filename) const {
  std::cout << "getting file " << filename << '\n';
  std::string name = getFilename(filename);
  return !::access(name.c_str(), F_OK) ? name : std::string{};
}

void FileCache::cacheFile(const FileBuffer &file) const {
  std::cout << "caching file\n";
  if (maxSize == 0)
    return;

  const std::string &filename = file.getNameIfAvailable();
  if (!filename.size())
    return;
  if (alwaysReject(filename))
    return;

  std::string name = getFilename(filename);
  if (!::access(name.c_str(), F_OK))
    return;

  while (cachedFiles.size() >= maxSize)
    removeOldest();

  assert(cachedFiles.size() < maxSize);
  int fd = ::creat(name.c_str(), 0644);
  if (fd == -1)
    return;
  if (!file.writeToFile(fd)) {
    ::unlink(name.c_str());
    return;
  }
  cachedFiles.emplace_back(name, std::chrono::system_clock::now());
}
