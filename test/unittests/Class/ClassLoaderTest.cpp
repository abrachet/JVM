
#include "JVM/Class/ClassLoader.h"
#include "gtest/gtest.h"

#include <fcntl.h>

TEST(Loader, NoExist) {
  auto [loadedClass, err] = ClassLoader::loadClass("No exist");
  ASSERT_EQ(err, std::string("Class 'No exist' does not exist."));
}

TEST(Loader, NoError) {
  auto [loadedClass, err] = ClassLoader::loadClass("Basic");
  ASSERT_TRUE(err.empty());
  EXPECT_EQ(loadedClass.second.location.type, ClassLocation::File);
  EXPECT_EQ(loadedClass.second.state, ClassLoader::Class::Loaded);
}

TEST(Loader, InvalidClassFile) {
  EXPECT_NE(::open("Test.class", O_CREAT, 0644), -1);
  auto [loadedClass, err] = ClassLoader::loadClass("Test");
  ASSERT_TRUE(err.empty());
  EXPECT_EQ(loadedClass.second.state, ClassLoader::Class::Erroneous);
}
