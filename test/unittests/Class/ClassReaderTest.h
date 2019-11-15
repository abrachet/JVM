
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
