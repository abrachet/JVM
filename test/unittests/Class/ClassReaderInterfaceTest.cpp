
#include "JVM/Class/ClassFileReader.h"
#include "gtest/gtest.h"
#include <fstream>
#include <unistd.h>

#include "ClassReaderTest.h"

class ClassReaderInterface : public ClassReaderBase {
  const char *getFilename() const override { return "Interface.class"; }
};

class ClassReaderImplements : public ClassReaderBase {
  const char *getFilename() const override { return "C.class"; }
};

constexpr int numCPEntries =
#include "Interface_NumCPEntries.inc"
    ;

TEST_F(ClassReaderInterface, NumConstPoolEntries) {
  auto &entries = classFile->getConstPool().getEntries();
  // Add 1, if the last number seen is 6, the number of entries is 7
  EXPECT_EQ(entries.size(), numCPEntries + 1);
}

constexpr int numInterfaces =
#include "Interface_NumInterface.inc"
    ;

TEST_F(ClassReaderImplements, Basic) {
  if (numInterfaces != -1)
    EXPECT_EQ(classFile->getInterfaces().size(), numInterfaces);
}
