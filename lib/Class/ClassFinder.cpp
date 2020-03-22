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

#include "JVM/Class/ClassFinder.h"
#include "JVM/Core/Defer.h"
#include <cassert>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <sys/mman.h>
#include <unistd.h>
#include <zip.h>

static std::pair<zip_t *, std::string> openZip(const std::string &path) {
  using namespace std::string_literals;
  int error = 0;
  zip_t *zip = ::zip_open(path.c_str(), ZIP_RDONLY, &error);
  switch (error) {
  case 0:
    break;
  case ZIP_ER_NOENT:
    return {nullptr, "Path '"s + path + "' does not exist."};
  case ZIP_ER_NOZIP:
  case ZIP_ER_OPEN:
    return {nullptr, "'"s + path + "' could not be opened."};
  default:
    errno = 0;
    return {nullptr, "Unkown error for path: '"s + path + "'."};
  }
  assert(zip);
  return {zip, {}};
}

std::string findRTJar(std::string &path) {
  const char *jhome = std::getenv("JAVA_HOME");
  if (!jhome)
    return "Can't find rt.jar: JAVA_HOME not set.";
  path = jhome;
  assert(!path.empty());
  if (path.back() != '/')
    path += '/';
  path += "jre/lib/rt.jar";

  auto [zip, err] = openZip(path);
  if (!zip) {
    path.clear();
    return err;
  }

  assert(err.empty());
  (void)::zip_close(zip);
  return {};
}

std::string registerFromJar(const std::string &path,
                            std::function<void(std::string)> registerFn) {
  auto [zip, err] = openZip(path.c_str());
  if (!zip) {
    assert(!err.empty());
    return err;
  }

  ssize_t numEntries = ::zip_get_num_entries(zip, 0);
  for (ssize_t i = 0; i < numEntries; i++)
    if (const char *str = zip_get_name(zip, i, ZIP_FL_ENC_GUESS); str)
      registerFn(str);

  (void)::zip_close(zip);
  return {};
}

static bool endsWith(const std::string &str, std::string_view search) {
  const char *data = str.c_str();
  assert(!search.data()[search.size()] && "search not null terminated");
  if (str.size() < search.size())
    return false;
  return !strcmp(data + str.size() - search.size(), search.data());
}

static bool jarContainsFile(std::string_view jarPath,
                            std::string_view className) {
  std::string str(jarPath);
  zip_t *zip = openZip(str).first;
  if (!zip)
    return false;
  auto _ = defer([zip] { zip_close(zip); });
  auto *ptr = zip_fopen(zip, className.data(), ZIP_FL_UNCHANGED);
  if (!ptr)
    return false;

  (void)::zip_fclose(ptr);
  return true;
}

// TODO: don't rely on file names stat the file to see if its a directory
// and look a the respective magic of a zip file and class file.
ClassLocation findClassLocation(std::string className,
                                const std::vector<std::string> &classPath) {
  assert(!endsWith(className, ".class") && "Invalid className");
  std::string classFilePath = className + ".class";
  for (const std::string &str : classPath) {
#ifdef JVM_CLASSFILE_CLASSPATH_EXTENSION
    if (endsWith(str, ".class") && endsWith(str, className + ".class"))
      return {std::move(className), str};
#endif
    if (endsWith(str, ".jar") && jarContainsFile(str, classFilePath))
      return {std::move(className), str, ClassLocation::InJar};

    std::string path = str;
    if (path.back() != '/')
      path += '/';
    path += classFilePath;
    if (!::access(path.c_str(), F_OK))
      return {std::move(className), std::move(path)};
  }

  return {};
}

ClassLocation findClassLocation(std::string className,
                                const std::vector<std::string> &classPath,
                                const FileCache &cache) {
  std::string fromCache = cache.getFileIfInCache(className);
  if (fromCache.size())
    return {std::move(className), std::move(fromCache)};
  return findClassLocation(std::move(className), classPath);
}

std::unique_ptr<FileBuffer> ZipFileBuffer::create(std::string_view zipFile,
                                                  std::string_view entry) {
  assert(!entry.data()[entry.size()] && "must be c-string");
  zip_t *zip = openZip(zipFile.data()).first;
  if (!zip)
    return nullptr;
  auto closeZip = defer([zip] { zip_close(zip); });
  zip_stat_t sb;
  zip_stat_init(&sb);
  if (zip_stat(zip, entry.data(), 0, &sb) == -1 || !(sb.valid & ZIP_STAT_SIZE))
    return nullptr;

  std::unique_ptr<ZipFileBuffer> ptr(new ZipFileBuffer);
  ptr->fileSize = sb.size;
  ptr->entry = entry;
  void *mapping = mmap(nullptr, sb.size + 1, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (mapping == MAP_FAILED)
    return nullptr;
  ptr->mappedFile = reinterpret_cast<char *>(mapping);

  zip_file_t *file = zip_fopen(zip, entry.data(), 0);
  if (!file)
    return nullptr;
  auto closeFile = defer([file] { zip_fclose(file); });
  if (zip_fread(file, mapping, sb.size) != sb.size)
    return nullptr;

  return ptr;
}

bool ZipFileBuffer::writeToFile(FileBuffer::FDType outFd) const {
  if (mappedFile == nullptr || fileSize == -1)
    return false;
  return ::write(outFd, mappedFile, fileSize) == fileSize;
}

ZipFileBuffer::~ZipFileBuffer() {
  if (mappedFile)
    munmap((void *)mappedFile, fileSize);
}
