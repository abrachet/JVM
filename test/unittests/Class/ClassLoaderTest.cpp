
#include "JVM/Class/ClassLoader.h"
#include "JVM/Class/ClassFinder.h"
#include "JVM/VM/Instructions.h"
#include "gtest/gtest.h"

#include <array>
#include <fcntl.h>
#include <thread>

TEST(Loader, NoExist) {
  auto classOrError = ClassLoader::loadClass("No exist");
  ASSERT_FALSE(classOrError);
  ASSERT_EQ(classOrError.getError(),
            std::string("Class 'No exist' does not exist."));
}

TEST(Loader, NoError) {
  if (ClassLoader::classPath.size() < 2) {
    std::string rtJar;
    ASSERT_FALSE(findRTJar(rtJar).size());
    ASSERT_TRUE(rtJar.size());
    ClassLoader::classPath.push_back(rtJar);
  }
  auto classOrError = ClassLoader::loadClass("Basic");
  ASSERT_TRUE(classOrError) << classOrError.getError();
  auto &loadedClass = classOrError.get();
  EXPECT_EQ(loadedClass.second.location.type, ClassLocation::File);
  EXPECT_EQ(loadedClass.second.state, ClassLoader::Class::Loaded);
}

TEST(Loader, InvalidClassFile) {
  EXPECT_NE(::open("Test.class", O_CREAT, 0644), -1);
  auto classOrError = ClassLoader::loadClass("Test");
  ASSERT_TRUE(classOrError) << classOrError.getError();
  EXPECT_EQ(classOrError.get().second.state, ClassLoader::Class::Erroneous);
}

TEST(Loader, LoadSuper) {
  if (ClassLoader::classPath.size() < 2) {
    std::string rtJar;
    ASSERT_FALSE(findRTJar(rtJar).size());
    ASSERT_TRUE(rtJar.size());
    ClassLoader::classPath.push_back(rtJar);
  }
  auto classOrError = ClassLoader::loadClass("InvD");
  ASSERT_TRUE(classOrError) << classOrError.getError();
  auto state = ClassLoader::findClassState("java/lang/Object");
  EXPECT_EQ(state, ClassLoader::Class::Loaded);
  EXPECT_EQ(classOrError.get().second.superClasses.size(), 1);
}

TEST(Loader, LoadInterfaces) {
  if (ClassLoader::classPath.size() < 2) {
    std::string rtJar;
    ASSERT_FALSE(findRTJar(rtJar).size());
    ASSERT_TRUE(rtJar.size());
    ClassLoader::classPath.push_back(rtJar);
  }
  auto classOrError = ClassLoader::loadClass("C");
  ASSERT_TRUE(classOrError) << classOrError.getError();
  auto state = ClassLoader::findClassState("java/lang/Object");
  EXPECT_EQ(state, ClassLoader::Class::Loaded);
  state = ClassLoader::findClassState("Interface");
  EXPECT_EQ(state, ClassLoader::Class::Loaded);
  EXPECT_EQ(classOrError.get().second.superClasses.size(), 2);
}

TEST(Loader, LoadMultiThread) {
  if (ClassLoader::classPath.size() < 2) {
    std::string rtJar;
    ASSERT_FALSE(findRTJar(rtJar).size());
    ASSERT_TRUE(rtJar.size());
    ClassLoader::classPath.push_back(rtJar);
  }
  auto classOrError = ClassLoader::loadClass("java/lang/Object");
  ASSERT_TRUE(classOrError) << classOrError.getError();
  int prevSize = ClassLoader::numLoadedClasses();
  std::array<std::thread, 4> threads;
  for (int i = 0; i < threads.size(); i++)
    threads[i] = std::thread([] { ClassLoader::loadClass("Methods"); });
  for (auto &thread : threads)
    thread.join();
  EXPECT_EQ(ClassLoader::numLoadedClasses(), prevSize + 1);
}

TEST(Loader, LoadMethod) {
  auto classOrError = ClassLoader::loadClass("Methods");
  ASSERT_TRUE(classOrError) << classOrError.getError();
  auto &classFile = classOrError.get().second.loadedClass;
  auto &methods = classFile->getMethods();
  auto &cpEntries = classFile->getConstPool().getEntries();
  ASSERT_EQ(methods[1].attributeCount, 1);
  int attrNameIdx = methods[1].attributes[0].attributeNameIndex;
  auto &utf8 =
      classFile->getConstPool().get<Class::ConstPool::Utf8Info>(attrNameIdx);
  ASSERT_EQ(std::string("Code"), std::string(utf8));
  using Class::CodeAttribute;
  CodeAttribute ca = CodeAttribute::fromAttr(methods[1].attributes[0]);
  EXPECT_EQ(ca.maxStack, 1);
  EXPECT_EQ(ca.maxLocals, 1);
  EXPECT_EQ(ca.exceptionTableLength, 0);
  EXPECT_EQ(ca.code[0], Instructions::iconst_1);
  EXPECT_EQ(ca.code[1], Instructions::ireturn);
}
