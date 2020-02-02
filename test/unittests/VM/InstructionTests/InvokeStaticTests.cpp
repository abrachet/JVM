
#include "JVM/Class/ClassLoader.h"
#include "JVM/VM/Instructions.h"
#include "JVM/VM/Stack.h"
#include "JVM/VM/ThreadContext.h"
#include "gtest/gtest.h"

static bool called;
static int firstArg;
static int secondArg;
extern "C" int Java_CallNative_add(void *, int arg1, int arg2) {
  called = true;
  firstArg = arg1;
  secondArg = arg2;
  return arg1 + arg2;
}

extern "C" int Java_java_PackagedClass_ret1(void *) {
  called = true;
  return 1;
}

static ThreadContext *currentContext;
extern "C" int Java_java_PackagedClass_testClassName(void *) {
  called = true;
  if (!currentContext)
    return (assert(0), 0);
  return currentContext->getCurrentClassName() == "java/PackagedClass";
}

struct InvokeStatic : public ::testing::Test {
  ThreadContext tc;

  InvokeStatic() : tc(std::move(*Stack::createStack(0x1000))) {
    using namespace std::literals;
    tc.pushFrame("CallNative"sv);
  }

  void SetUp() override {
    if (ClassLoader::classPath.size() < 2) {
      std::string rtJar;
      ASSERT_FALSE(findRTJar(rtJar).size());
      ASSERT_TRUE(rtJar.size());
      ClassLoader::classPath.push_back(rtJar);
    }
    auto classOrError = ClassLoader::loadClass(tc.getCurrentClassName());
    ASSERT_TRUE(classOrError) << classOrError.getError();
    auto &classFile = classOrError.get().second.loadedClass;
    auto &methods = classFile->getMethods();
    EXPECT_EQ(methods.size(), 5);
  }

  void setUpCallAdd() {
    auto classOrError = ClassLoader::loadClass(tc.getCurrentClassName());
    ASSERT_TRUE(classOrError) << classOrError.getError();
    auto &classFile = classOrError.get().second.loadedClass;
    auto &methods = classFile->getMethods();
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

  void setUpCallRet1() {
    auto classOrError = ClassLoader::loadClass(tc.getCurrentClassName());
    ASSERT_TRUE(classOrError) << classOrError.getError();
    auto &classFile = classOrError.get().second.loadedClass;
    auto &methods = classFile->getMethods();
    ASSERT_EQ(methods[3].attributeCount, 1);
    int attrNameIdx = methods[3].attributes[0].attributeNameIndex;
    auto &utf8 =
        classFile->getConstPool().get<Class::ConstPool::Utf8Info>(attrNameIdx);
    ASSERT_EQ(std::string("Code"), std::string(utf8));
    using Class::CodeAttribute;
    CodeAttribute ca = CodeAttribute::fromAttr(methods[3].attributes[0]);
    tc.pc = ca.code;
    EXPECT_EQ(ca.code[0], Instructions::invokestatic);
  }

  void setUpCallTestName() {
    auto classOrError = ClassLoader::loadClass(tc.getCurrentClassName());
    ASSERT_TRUE(classOrError) << classOrError.getError();
    auto &classFile = classOrError.get().second.loadedClass;
    auto &methods = classFile->getMethods();
    ASSERT_EQ(methods[4].attributeCount, 1);
    int attrNameIdx = methods[4].attributes[0].attributeNameIndex;
    auto &utf8 =
        classFile->getConstPool().get<Class::ConstPool::Utf8Info>(attrNameIdx);
    ASSERT_EQ(std::string("Code"), std::string(utf8));
    using Class::CodeAttribute;
    CodeAttribute ca = CodeAttribute::fromAttr(methods[4].attributes[0]);
    tc.pc = ca.code;
    EXPECT_EQ(ca.code[0], Instructions::invokestatic);
  }
};

TEST_F(InvokeStatic, BasicNative) {
  setUpCallAdd();
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
  EXPECT_EQ(firstArg, 1);
  EXPECT_EQ(secondArg, 2);
}

TEST_F(InvokeStatic, PackagedNativeName) {
  setUpCallRet1();
  tc.callNext();
  EXPECT_EQ(tc.stack.pop<1>(), 1);
}

TEST_F(InvokeStatic, NewFrameForNative) {
  setUpCallTestName();
  currentContext = &tc;
  called = false;
  tc.callNext();
  EXPECT_TRUE(called);
  EXPECT_EQ(tc.stack.pop<1>(), 1);
}
