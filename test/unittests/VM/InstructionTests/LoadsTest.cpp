
#include "InstructionTests.h"
#include "JVM/VM/Instructions.h"
#include "JVM/VM/Stack.h"
#include "JVM/VM/ThreadContext.h"
#include "gtest/gtest.h"

struct Loads : public MethodCaller<> {

  std::string_view getClassName() override { return "General"; }

  void setUpTestILoad() {
    setUpMethod(2);
    EXPECT_EQ(getCode()[0], Instructions::iconst_5);
    EXPECT_EQ(getCode()[1], Instructions::istore_0);
    EXPECT_EQ(getCode()[2], Instructions::iload_0);
    EXPECT_EQ(getCode()[3], Instructions::invokestatic);
    EXPECT_EQ(getCode()[6], Instructions::istore_1);
  }
};

extern int returnArgGot;

TEST_F(Loads, Iload) {
  setUpTestILoad();
  // Manually make room for two variables.
  tc.stack.push<1>(10001);
  tc.stack.push<1>(10002);
  callMultiple(4);
  EXPECT_EQ(returnArgGot, 5);
  tc.callNext();                    // Store in 1
  uint64_t got = tc.stack.pop<1>(); // Pop off 1
  EXPECT_EQ(got, 5);
  got = tc.stack.pop<1>(); // Now pop off the local in 0
  EXPECT_EQ(got, 5);
}
