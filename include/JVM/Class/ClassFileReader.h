
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

public:
  ClassFileReader(const std::string& filename)
      : reader(FileReader<true>::create(filename)) {}

  using ClassFileOrError = std::pair<std::unique_ptr<ClassFile>, std::string>;

  std::unique_ptr<FileBuffer> takeFileBuffer() {
    auto buffer = reader->takeFileBuffer();
    reader = nullptr;
    return buffer;
  }

  ClassFileOrError read();
};

#endif // JVM_CLASS_CLASSFILEREADER_H
