
#ifndef JVM_CLASS_CLASSFILEREADER_H
#define JVM_CLASS_CLASSFILEREADER_H

#include <memory>
#include <string>
#include <utility>
#include "JVM/Class/ClassFile.h"
#include "JVM/Core/FileReader.h"

class ClassFileReader {
  std::unique_ptr<FileReader<true>> reader;

 public:
  ClassFileReader(std::string_view filename)
      : reader(FileReader<true>::create(filename)) {}

  using ClassFileOrError = std::pair<std::unique_ptr<ClassFile>, std::string>;

  ClassFileOrError read();
};

#endif  // JVM_CLASS_CLASSFILEREADER_H
