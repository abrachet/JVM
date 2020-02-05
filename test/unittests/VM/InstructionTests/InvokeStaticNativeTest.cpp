
#include "InstructionTests.h"
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

struct InvokeStaticNative : public MethodCaller<> {

  std::string_view getClassName() override { return "CallNative"; }

  void setUpCallAdd() {
    setUpMethod(2);
    EXPECT_EQ(getCode()[0], Instructions::iconst_1);
    EXPECT_EQ(getCode()[1], Instructions::iconst_2);
    EXPECT_EQ(getCode()[2], Instructions::invokestatic);
  }

  void setUpCallRet1() {
    setUpMethod(3);
    EXPECT_EQ(getCode()[0], Instructions::invokestatic);
  }

  void setUpCallTestName() {
    setUpMethod(4);
    EXPECT_EQ(getCode()[0], Instructions::invokestatic);
  }
};

TEST_F(InvokeStaticNative, BasicNative) {
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
  EXPECT_EQ(*getCode(), Instructions::istore_0);
}

TEST_F(InvokeStaticNative, PackagedNativeName) {
  setUpCallRet1();
  tc.callNext();
  EXPECT_EQ(tc.stack.pop<1>(), 1);
  EXPECT_EQ(*getCode(), Instructions::istore_0);
}

TEST_F(InvokeStaticNative, NewFrameForNative) {
  setUpCallTestName();
  currentContext = &tc;
  called = false;
  tc.callNext();
  EXPECT_TRUE(called);
  EXPECT_EQ(tc.stack.pop<1>(), 1);
  EXPECT_EQ(*getCode(), Instructions::istore_0);
}
