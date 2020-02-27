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

#ifndef JVM_CLASS_CLASSFILEREADER_H
#define JVM_CLASS_CLASSFILEREADER_H

#include "JVM/Class/ClassFile.h"
#include "JVM/Core/FileReader.h"
#include "JVM/Core/decimal.h"
#include <memory>
#include <string>
#include <utility>

class ClassFileReader {
  std::unique_ptr<FileReader<true>> reader;

  std::string readConstPool(ClassFile &);
  std::string readCPUtf8(ClassFile &);
  template <typename IntT, Class::ConstPool::Type CpType>
  std::string readCPIntegeral(ClassFile &);
  std::string readCPClass(ClassFile &);
  std::string readCPString(ClassFile &);
  template <Class::ConstPool::Type CpType> std::string readCPRef(ClassFile &);
  std::string readCPNameType(ClassFile &);
  std::string readCPMethodHandle(ClassFile &);
  std::string readCPMethodType(ClassFile &);
  std::string readCPInvokeDyn(ClassFile &);

  std::string readInterfaces(ClassFile &);

  std::string readFields(ClassFile &);

  std::string readAttributes(Class::Attributes &);

  std::string readMethod(Class::Method &);
  std::string readMethods(ClassFile &);

public:
  ClassFileReader(const std::string &filename)
      : reader(FileReader<true>::create(filename)) {}

  ClassFileReader(std::unique_ptr<FileBuffer> fileBuffer)
      : reader(FileReader<true>::create(std::move(fileBuffer))) {}

  using ClassFileOrError = std::pair<std::unique_ptr<ClassFile>, std::string>;

  std::unique_ptr<FileBuffer> takeFileBuffer() {
    auto buffer = reader->takeFileBuffer();
    reader = nullptr;
    return buffer;
  }

  ClassFileOrError read();
};

#endif // JVM_CLASS_CLASSFILEREADER_H
