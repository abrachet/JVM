
#include "InstructionTests.h"
#include "JVM/Class/ClassLoader.h"
#include "JVM/VM/Instructions.h"
#include "JVM/VM/Stack.h"
#include "JVM/VM/ThreadContext.h"
#include "gtest/gtest.h"

struct InvokeStaticJVM : public MethodCaller<> {

  std::string_view getClassName() override { return "General"; }

  void setUpCallReturnArgJVM() {
    setUpMethod(4);
    EXPECT_EQ(getCode()[0], Instructions::iconst_1);
    EXPECT_EQ(getCode()[1], Instructions::invokestatic);
    EXPECT_EQ(getCode()[4], Instructions::istore_0);
  }
};

TEST_F(InvokeStaticJVM, Basic) {
  setUpCallReturnArgJVM();
  void *stackStart = tc.stack.sp;
  ASSERT_EQ(tc.getMethodName(), "callReturnArgJVM");
  ASSERT_EQ(tc.getMethodTypeName(), "()V");
  callMultiple(2);
  ASSERT_EQ(tc.numFrames(), 2);
  // These end up being the same because the previous frame only had one entry
  // which was the argument to this method.
  ASSERT_EQ(stackStart, tc.currentFrame().frameStart);
  ASSERT_EQ(tc.getMethodName(), "returnArgJVM");
  ASSERT_EQ(tc.getMethodTypeName(), "(I)I");
  EXPECT_EQ(getCode()[0], Instructions::iload_0);
  tc.callNext();
  uint64_t one = tc.stack.pop<1>();
  EXPECT_EQ(one, 1);
  tc.stack.push<1>(one);
}
