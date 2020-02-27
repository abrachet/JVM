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

#include "JVM/Class/ClassFileReader.h"
#include "gtest/gtest.h"
#include <memory>

class ClassReaderBase : public testing::Test {
  virtual const char *getFilename() const = 0;

  void SetUp() override {
    ClassFileReader reader(getFilename());
    auto fileOrError = reader.read();
    ASSERT_EQ(fileOrError.second, std::string());
    classFile = std::move(fileOrError.first);
    buffer = reader.takeFileBuffer();
  }

  std::unique_ptr<FileBuffer> buffer;

public:
  std::unique_ptr<ClassFile> classFile;
};
