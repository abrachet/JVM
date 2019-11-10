
#include <fstream>
#include "JVM/Core/FileReader.h"
#include "gtest/gtest.h"

class FileReaderTest : public ::testing::Test {
  void SetUp() override { f.open(filename, std::ios_base::trunc); }

  void TearDown() override { f.close(); }

 public:
  ~FileReaderTest() override { std::remove(filename); }

  std::ofstream f;
  static constexpr char filename[] = "test.tmp";
};

TEST_F(FileReaderTest, ReadInt) {
  int a = 5;
  f.write(reinterpret_cast<const char*>(&a), sizeof(int));
  f.close();

  auto reader = FileReader<false>::create(filename);
  int read;
  reader->read(read);
  EXPECT_EQ(read, a);
  EXPECT_EQ(reader->getPos(), sizeof(int));
}

TEST_F(FileReaderTest, ReadPointer) {
  int a = 5;
  f.write(reinterpret_cast<const char*>(&a), sizeof(int));
  f.close();

  auto reader = FileReader<false>::create(filename);
  int* read = nullptr;
  reader->read(read);
  ASSERT_NE(read, nullptr);
  EXPECT_EQ(*read, a);
  EXPECT_EQ(reader->getPos(), sizeof(int));
  EXPECT_EQ(reinterpret_cast<const char*>(read), reader->data());
}

TEST_F(FileReaderTest, ReadMultiple) {
  int arr[5] = {10, 4, 5, 10548845, 3178941};
  f.write(reinterpret_cast<const char*>(arr), sizeof(arr));
  f.close();

  auto reader = FileReader<false>::create(filename);
  for (int a, i = 0; i < 5; i++) {
    EXPECT_TRUE(reader->read(a));
    EXPECT_EQ(a, arr[i]);
  }
  int a = 438413;
  EXPECT_FALSE(reader->read(a));
  EXPECT_EQ(a, 438413);
}

TEST_F(FileReaderTest, DifferentTypes) {
  int a = 23489071, ea;
  uint8_t b = 34, eb;
  uint64_t c = 3487587475827, ec;

  f.write(reinterpret_cast<const char*>(&a), sizeof(a));
  f.write(reinterpret_cast<const char*>(&b), sizeof(b));
  f.write(reinterpret_cast<const char*>(&c), sizeof(c));
  f.close();

  auto reader = FileReader<false>::create(filename);
  EXPECT_TRUE(reader->read(ea));
  EXPECT_EQ(ea, a);
  EXPECT_TRUE(reader->read(eb));
  EXPECT_EQ(eb, b);
  EXPECT_TRUE(reader->read(ec));
  EXPECT_EQ(ec, c);
}

TEST_F(FileReaderTest, TakeFile) {
  f.write("abc", 3);
  f.close();

  auto reader = FileReader<false>::create(filename);
  char c;
  reader->read(c);
  EXPECT_EQ(c, 'a');
  std::unique_ptr<FileBuffer> file = reader->takeFileBuffer();
  EXPECT_FALSE(reader->read(c));
  EXPECT_EQ(c, 'a');
  EXPECT_EQ(reader->takeFileBuffer(), nullptr);

  // This is safe because FileBuffer always allocates 1 extra 0 byte.
  EXPECT_STREQ(*file, "abc");
}

TEST_F(FileReaderTest, Pos) {
  f.write("abc", 3);
  f.close();

  auto reader = FileReader<false>::create(filename);
  char c;
  ASSERT_TRUE(reader->read(c));
  EXPECT_EQ(c, 'a');
  EXPECT_EQ(reader->getPos(), 1);
  reader->seek(0);
  EXPECT_EQ(reader->getPos(), 0);
  ASSERT_TRUE(reader->read(c));
  EXPECT_EQ(c, 'a');
  EXPECT_EQ(reader->getPos(), 1);
}

TEST_F(FileReaderTest, Endianness) {
  uint8_t arr[] = {0xCA, 0xFE, 0xBA, 0xBE};
  f.write(reinterpret_cast<const char*>(arr), 1);
  f.write(reinterpret_cast<const char*>(arr + 1), 1);
  f.write(reinterpret_cast<const char*>(arr + 2), 1);
  f.write(reinterpret_cast<const char*>(arr + 3), 1);
  f.close();

  auto reader = FileReader<true>::create(filename);
  uint32_t magic = 0;
  ASSERT_TRUE(reader->read(magic));
  EXPECT_EQ(magic, 0xCAFEBABE);
}
