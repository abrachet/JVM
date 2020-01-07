
#include "JVM/Class/ClassLoader.h"
#include "gtest/gtest.h"

TEST(Loader, NoExist) {
  auto [loadedClass, err] = ClassLoader::loadClass("No exist");
  ASSERT_EQ(err, std::string("Class 'No exist' does not exist."));
  ASSERT_FALSE(loadedClass);
}

TEST(Loader, NoError) {
  auto [loadedClass, err] = ClassLoader::loadClass("Basic");
  ASSERT_TRUE(err.empty());
}
