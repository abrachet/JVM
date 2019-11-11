
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

public:
  ClassFileReader(std::string_view filename)
      : reader(FileReader<true>::create(filename)) {}

  using ClassFileOrError = std::pair<std::unique_ptr<ClassFile>, std::string>;

  ClassFileOrError read();
};

#endif // JVM_CLASS_CLASSFILEREADER_H
