
#include "JVM/VM/Allocator.h"
#include "JVM/VM/ClassLoader.h"
#include "gtest/gtest.h"

TEST(Allocator, Basic) {
  if (ClassLoader::classPath.size() < 2) {
    std::string rtJar;
    ASSERT_FALSE(findRTJar(rtJar).size());
    ASSERT_TRUE(rtJar.size());
    ClassLoader::classPath.push_back(rtJar);
  }
  auto classOrError = ClassLoader::loadClass("ObjectRepresentationIJ");
  ASSERT_TRUE(classOrError) << classOrError.getError();
  InMemoryObject *obj = jvm::allocate(classOrError->second);
  EXPECT_EQ(obj->className, "ObjectRepresentationIJ");
  uint64_t *IJ = reinterpret_cast<uint64_t *>(obj + 1);
  EXPECT_EQ(IJ[0], 0);
  EXPECT_EQ(IJ[1], 0);
  // Test that writing doesn't cause a fault or upset sanitizers.
  IJ[0] = 100;
  IJ[0] = 100;
  jvm::deallocate(obj);
}
