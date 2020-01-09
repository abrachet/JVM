
#include "JVM/Class/ClassLoader.h"
#include "JVM/Class/ClassFinder.h"
#include "gtest/gtest.h"

#include <fcntl.h>

TEST(Loader, NoExist) {
  auto [loadedClass, err] = ClassLoader::loadClass("No exist");
  ASSERT_EQ(err, std::string("Class 'No exist' does not exist."));
}

TEST(Loader, NoError) {
  if (ClassLoader::classPath.size() < 2) {
    std::string rtJar;
    ASSERT_FALSE(findRTJar(rtJar).size());
    ASSERT_TRUE(rtJar.size());
    ClassLoader::classPath.push_back(rtJar);
  }
  auto [loadedClass, err] = ClassLoader::loadClass("Basic");
  ASSERT_TRUE(err.empty()) << err;
  EXPECT_EQ(loadedClass.second.location.type, ClassLocation::File);
  EXPECT_EQ(loadedClass.second.state, ClassLoader::Class::Loaded);
}

TEST(Loader, InvalidClassFile) {
  EXPECT_NE(::open("Test.class", O_CREAT, 0644), -1);
  auto [loadedClass, err] = ClassLoader::loadClass("Test");
  ASSERT_TRUE(err.empty()) << err;
  EXPECT_EQ(loadedClass.second.state, ClassLoader::Class::Erroneous);
}

TEST(Loader, LoadSuper) {
  if (ClassLoader::classPath.size() < 2) {
    std::string rtJar;
    ASSERT_FALSE(findRTJar(rtJar).size());
    ASSERT_TRUE(rtJar.size());
    ClassLoader::classPath.push_back(rtJar);
  }
  auto [loadedClass, err] = ClassLoader::loadClass("InvD");
  ASSERT_TRUE(err.empty()) << err;
  auto state = ClassLoader::findClassState("java/lang/Object");
  EXPECT_EQ(state, ClassLoader::Class::Loaded);
}
