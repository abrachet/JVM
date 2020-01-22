
#include "JVM/Class/ClassLoader.h"
#include "JVM/VM/Instructions.h"
#include "JVM/VM/Stack.h"
#include "JVM/VM/ThreadContext.h"
#include "gtest/gtest.h"

static bool called = false;
extern "C" int Java_CallNative_add(void *, int arg1, int arg2) {
  called = true;
  return arg1 + arg2;
}

struct InvokeStatic : public ::testing::Test {
  ThreadContext tc;

  InvokeStatic() : tc(std::move(*Stack::createStack(0x1000))) {
    tc.loadedClassName = "CallNative";
  }

  void SetUp() override {
    if (ClassLoader::classPath.size() < 2) {
      std::string rtJar;
      ASSERT_FALSE(findRTJar(rtJar).size());
      ASSERT_TRUE(rtJar.size());
      ClassLoader::classPath.push_back(rtJar);
    }
    auto classOrError = ClassLoader::loadClass(tc.loadedClassName);
    ASSERT_TRUE(classOrError) << classOrError.getError();
    auto &classFile = classOrError.get().second.loadedClass;
    auto &methods = classFile->getMethods();
    ASSERT_EQ(methods.size(), 3);
    ASSERT_EQ(methods[2].attributeCount, 1);
    int attrNameIdx = methods[2].attributes[0].attributeNameIndex;
    auto &utf8 =
        classFile->getConstPool().get<Class::ConstPool::Utf8Info>(attrNameIdx);
    ASSERT_EQ(std::string("Code"), std::string(utf8));
    using Class::CodeAttribute;
    CodeAttribute ca = CodeAttribute::fromAttr(methods[2].attributes[0]);
    tc.pc = ca.code;
    ASSERT_TRUE(tc.pc);
    EXPECT_EQ(ca.code[0], Instructions::iconst_1);
    EXPECT_EQ(ca.code[1], Instructions::iconst_2);
    EXPECT_EQ(ca.code[2], Instructions::invokestatic);
  }
};

TEST_F(InvokeStatic, Native) {
  tc.callNext();
  tc.callNext();
  uint64_t two = tc.stack.pop<1>();
  ASSERT_EQ(two, 2);
  uint64_t one = tc.stack.pop<1>();
  ASSERT_EQ(one, 1);
  tc.stack.push<1>(1);
  tc.stack.push<1>(2);
  called = false;
  tc.callNext();
  EXPECT_EQ(tc.stack.pop<1>(), 3);
  EXPECT_TRUE(called);
}
