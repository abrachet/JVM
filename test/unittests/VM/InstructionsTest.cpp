
#include "JVM/VM/Instructions.h"
#include "JVM/VM/Stack.h"
#include "JVM/VM/ThreadContext.h"
#include "gtest/gtest.h"
#include <memory>

struct Ins : public testing::Test {
  std::unique_ptr<ThreadContext> threadContext;

  Ins() {
    ErrorOr<Stack> stack = Stack::createStack(0x1000);
    assert(stack);
    threadContext = std::make_unique<ThreadContext>(std::move(*stack));
  }

  void SetUp() override {
    threadContext->stack.sp = threadContext->stack.stack;
  }
};

TEST_F(Ins, Nop) {
  uint8_t instructions[] = {Instructions::nop, 0};
  threadContext->pc = instructions;
  threadContext->callNext();
  EXPECT_EQ(threadContext->pc, instructions + 1);
  EXPECT_EQ(threadContext->stack.sp, threadContext->stack.stack);
}
