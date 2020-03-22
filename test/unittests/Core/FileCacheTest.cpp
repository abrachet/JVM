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
#include "JVM/Core/FileBuffer.h"
#include "gtest/gtest.h"
#include <cerrno>
#include <fstream>

struct FileCacheTest : public ::testing::Test {
  static std::unique_ptr<FileBuffer> getFile(std::string_view name) {
    std::string str(name.data(), name.size());
    std::ofstream f(str);
    f.write("text", 4);
    f.close();
    auto ptr = FileBuffer::create(str);
    assert(ptr);
    return ptr;
  }

  static constexpr std::string_view cacheDir() { return "./testcache"; }

  static bool cacheDirExists() { return !::access(cacheDir().data(), F_OK); }

  static void removeDir(const std::string &str) {
    std::string rm = "rm -rf " + str;
    ::system(rm.c_str());
  }

  void SetUp() override { ASSERT_FALSE(cacheDirExists()); }

  void TearDown() override {
    removeDir({cacheDir().data(), cacheDir().size()});
  }
};

TEST_F(FileCacheTest, EmptyCache) {
  FileCache cache(cacheDir(), 0);
  ASSERT_TRUE(cacheDirExists());
  std::unique_ptr<FileBuffer> file = getFile("file1");
  cache.cacheFile(*file);
  std::string got = cache.getFileIfInCache("file1");
  EXPECT_EQ(got.size(), 0);
}

TEST_F(FileCacheTest, CacheFile) {
  FileCache cache(cacheDir());
  std::unique_ptr<FileBuffer> file = getFile("file1");
  cache.cacheFile(*file);
  std::string got = cache.getFileIfInCache("file1");
  EXPECT_EQ(got, "./testcache/file1");
}

TEST_F(FileCacheTest, Evict) {
  FileCache cache(cacheDir(), 1);
  std::unique_ptr<FileBuffer> file1 = getFile("file1");
  cache.cacheFile(*file1);
  std::unique_ptr<FileBuffer> file2 = getFile("file2");
  cache.cacheFile(*file2);
  std::string got = cache.getFileIfInCache("file1");
  EXPECT_EQ(got.size(), 0);
}

TEST_F(FileCacheTest, Directory) {
  FileCache cache(cacheDir());
  // Can't be bothered removing this.
  ::mkdir("dir", 0700);
  ASSERT_EQ(::access("dir", F_OK), 0);
  std::unique_ptr<FileBuffer> file = getFile("dir/file");
  cache.cacheFile(*file);
  std::string got = cache.getFileIfInCache("dir/file");
  EXPECT_EQ(got, "./testcache/dir._~_.file");
  EXPECT_EQ(::access(got.c_str(), F_OK), 0);
}
