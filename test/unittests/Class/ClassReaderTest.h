
#include "JVM/Class/ClassFileReader.h"
#include "gtest/gtest.h"
#include <memory>

class ClassReader : public testing::Test {

  void SetUp() override {
    ClassFileReader reader(filename);
    auto fileOrError = reader.read();
    ASSERT_EQ(fileOrError.second, std::string());
    classFile = std::move(fileOrError.first);
    buffer = reader.takeFileBuffer();
  }

  std::unique_ptr<FileBuffer> buffer;

public:
  std::unique_ptr<ClassFile> classFile;
};
