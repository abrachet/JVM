
#include "JVM/VM/ClassLoader.h"
#include "gtest/gtest.h"

TEST(ClassLoader, ObjectRepresentation) {
  if (ClassLoader::classPath.size() < 2) {
    std::string rtJar;
    ASSERT_FALSE(findRTJar(rtJar).size());
    ASSERT_TRUE(rtJar.size());
    ClassLoader::classPath.push_back(rtJar);
  }
  auto classOrError = ClassLoader::loadClass("ObjectRepresentationIJ");
  ASSERT_TRUE(classOrError) << classOrError.getError();
  const auto &objectRep = classOrError->second.objectRepresentation;
  EXPECT_EQ(objectRep.getObjectSize(), 16);
  EXPECT_EQ(objectRep.getFieldOffset(0), 0);
  EXPECT_EQ(objectRep.getFieldOffset(1), 8);
}
