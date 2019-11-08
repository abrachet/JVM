
#include "JVM/Core/FileBuffer.h"
#include <fstream>
#include "gtest/gtest.h"

TEST(FileBuffer, OpenFile) {
  std::ofstream f("test.tmp");
  f.write("text", 4);
  f.close();

  std::unique_ptr<FileBuffer> buf = FileBuffer::create("test.tmp");
  EXPECT_NE(buf.get(), nullptr);
}
