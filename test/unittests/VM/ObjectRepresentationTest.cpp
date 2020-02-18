
#include "JVM/VM/ObjectRepresentation.h"
#include "JVM/Class/ClassFileReader.h"
#include "gtest/gtest.h"
#include <memory>

class ObjectRepresentationTest : public ::testing::Test {
  virtual const char *getFilename() const = 0;

  void SetUp() override {
    ClassFileReader reader(getFilename());
    auto fileOrError = reader.read();
    ASSERT_EQ(fileOrError.second, std::string());
    classFile = std::move(fileOrError.first);
    buffer = reader.takeFileBuffer();
    orOrError = ObjectRepresentation::createFromClassFile(*classFile);
  }

  ErrorOr<ObjectRepresentation> orOrError = std::string("error");
  std::unique_ptr<FileBuffer> buffer;
  std::unique_ptr<ClassFile> classFile;

public:
  const ObjectRepresentation &getOR() const {
    assert(orOrError);
    return *orOrError;
  }
};

class ObjectRepresentationEmpty : public ObjectRepresentationTest {
  const char *getFilename() const override {
    return "ObjectRepresentationEmpty.class";
  }
};

TEST_F(ObjectRepresentationEmpty, Empty) {
  EXPECT_EQ(getOR().getObjectSize(), 0);
}

class ObjectRepresentationII : public ObjectRepresentationTest {
  const char *getFilename() const override {
    return "ObjectRepresentationII.class";
  }
};

TEST_F(ObjectRepresentationII, II) {
  EXPECT_EQ(getOR().getObjectSize(), 8);
  EXPECT_EQ(getOR().getFieldOffset(0), 0);
  EXPECT_EQ(getOR().getFieldOffset(1), 4);
}

class ObjectRepresentationIJ : public ObjectRepresentationTest {
  const char *getFilename() const override {
    return "ObjectRepresentationIJ.class";
  }
};

TEST_F(ObjectRepresentationIJ, IJ) {
  EXPECT_EQ(getOR().getObjectSize(), 16);
  EXPECT_EQ(getOR().getFieldOffset(0), 0);
  EXPECT_EQ(getOR().getFieldOffset(1), 8);
}

class ObjectRepresentationJI : public ObjectRepresentationTest {
  const char *getFilename() const override {
    return "ObjectRepresentationJI.class";
  }
};

TEST_F(ObjectRepresentationJI, JI) {
  EXPECT_EQ(getOR().getObjectSize(), 12);
  EXPECT_EQ(getOR().getFieldOffset(0), 0);
  EXPECT_EQ(getOR().getFieldOffset(1), 8);
}

class ObjectRepresentationL : public ObjectRepresentationTest {
  const char *getFilename() const override {
    return "ObjectRepresentationL.class";
  }
};

TEST_F(ObjectRepresentationL, L) {
  EXPECT_EQ(getOR().getObjectSize(), 8);
  EXPECT_EQ(getOR().getFieldOffset(0), 0);
}