
#include "JVM/VM/Instructions.h"
#include "JVM/Core/float_cast.h"
#include "JVM/VM/Stack.h"
#include "JVM/VM/ThreadContext.h"
#include "gtest/gtest.h"
#include <memory>

struct Ins : public testing::Test {
  std::unique_ptr<ThreadContext> threadContext;
  void *stackStart;

  Ins() {
    ErrorOr<Stack> stack = Stack::createStack(0x1000);
    assert(stack);
    threadContext = std::make_unique<ThreadContext>(std::move(*stack));
    stackStart = (char *)threadContext->stack.stack + threadContext->stack.size;
  }

  void SetUp() override { threadContext->stack.sp = stackStart; }
};

TEST_F(Ins, Nop) {
  uint8_t instructions[] = {Instructions::nop, 0};
  threadContext->pc = instructions;
  threadContext->callNext();
  EXPECT_EQ(threadContext->pc, instructions + 1);
  EXPECT_EQ(threadContext->stack.sp, stackStart);
}

TEST_F(Ins, Aconst_null) {
  uint8_t instructions[] = {Instructions::aconst_null, 0};
  threadContext->pc = instructions;
  threadContext->callNext();
  EXPECT_EQ(threadContext->pc, instructions + 1);
  EXPECT_EQ(threadContext->stack.sp, (uint64_t *)stackStart - 1);
  uint64_t pop = threadContext->stack.pop<2>();
  EXPECT_EQ(pop, 0);
  EXPECT_EQ(threadContext->stack.sp, stackStart);
}

TEST_F(Ins, Iconst) {
  uint8_t instructions[] = {Instructions::iconst_m1, 0};
  threadContext->pc = instructions;
  threadContext->callNext();
  EXPECT_EQ(threadContext->pc, instructions + 1);
  EXPECT_EQ(threadContext->stack.sp, (uint32_t *)stackStart - 1);
  int32_t pop = static_cast<int32_t>(threadContext->stack.pop<1>());
  EXPECT_EQ(pop, -1);
  EXPECT_EQ(threadContext->stack.sp, stackStart);

  instructions[0] = Instructions::iconst_0;
  threadContext->pc = instructions;
  threadContext->callNext();
  EXPECT_EQ(threadContext->pc, instructions + 1);
  EXPECT_EQ(threadContext->stack.sp, (uint32_t *)stackStart - 1);
  pop = static_cast<int32_t>(threadContext->stack.pop<1>());
  EXPECT_EQ(pop, 0);
  EXPECT_EQ(threadContext->stack.sp, stackStart);

  instructions[0] = Instructions::iconst_1;
  threadContext->pc = instructions;
  threadContext->callNext();
  EXPECT_EQ(threadContext->pc, instructions + 1);
  EXPECT_EQ(threadContext->stack.sp, (uint32_t *)stackStart - 1);
  pop = static_cast<int32_t>(threadContext->stack.pop<1>());
  EXPECT_EQ(pop, 1);
  EXPECT_EQ(threadContext->stack.sp, stackStart);

  instructions[0] = Instructions::iconst_2;
  threadContext->pc = instructions;
  threadContext->callNext();
  EXPECT_EQ(threadContext->pc, instructions + 1);
  EXPECT_EQ(threadContext->stack.sp, (uint32_t *)stackStart - 1);
  pop = static_cast<int32_t>(threadContext->stack.pop<1>());
  EXPECT_EQ(pop, 2);
  EXPECT_EQ(threadContext->stack.sp, stackStart);

  instructions[0] = Instructions::iconst_3;
  threadContext->pc = instructions;
  threadContext->callNext();
  EXPECT_EQ(threadContext->pc, instructions + 1);
  EXPECT_EQ(threadContext->stack.sp, (uint32_t *)stackStart - 1);
  pop = static_cast<int32_t>(threadContext->stack.pop<1>());
  EXPECT_EQ(pop, 3);
  EXPECT_EQ(threadContext->stack.sp, stackStart);

  instructions[0] = Instructions::iconst_4;
  threadContext->pc = instructions;
  threadContext->callNext();
  EXPECT_EQ(threadContext->pc, instructions + 1);
  EXPECT_EQ(threadContext->stack.sp, (uint32_t *)stackStart - 1);
  pop = static_cast<int32_t>(threadContext->stack.pop<1>());
  EXPECT_EQ(pop, 4);
  EXPECT_EQ(threadContext->stack.sp, stackStart);

  instructions[0] = Instructions::iconst_5;
  threadContext->pc = instructions;
  threadContext->callNext();
  EXPECT_EQ(threadContext->pc, instructions + 1);
  EXPECT_EQ(threadContext->stack.sp, (uint32_t *)stackStart - 1);
  pop = static_cast<int32_t>(threadContext->stack.pop<1>());
  EXPECT_EQ(pop, 5);
  EXPECT_EQ(threadContext->stack.sp, stackStart);
}

TEST_F(Ins, Lconst) {
  uint8_t instructions[] = {Instructions::lconst_0, 0};
  threadContext->pc = instructions;
  threadContext->callNext();
  EXPECT_EQ(threadContext->pc, instructions + 1);
  EXPECT_EQ(threadContext->stack.sp, (uint64_t *)stackStart - 1);
  uint64_t pop = threadContext->stack.pop<2>();
  EXPECT_EQ(pop, 0);
  EXPECT_EQ(threadContext->stack.sp, stackStart);

  instructions[0] = Instructions::lconst_1;
  threadContext->pc = instructions;
  threadContext->callNext();
  EXPECT_EQ(threadContext->pc, instructions + 1);
  EXPECT_EQ(threadContext->stack.sp, (uint64_t *)stackStart - 1);
  pop = threadContext->stack.pop<2>();
  EXPECT_EQ(pop, 1);
  EXPECT_EQ(threadContext->stack.sp, stackStart);
}

TEST_F(Ins, Fconst) {
  uint8_t instructions[] = {Instructions::fconst_0, 0};
  threadContext->pc = instructions;
  threadContext->callNext();
  EXPECT_EQ(threadContext->pc, instructions + 1);
  EXPECT_EQ(threadContext->stack.sp, (uint32_t *)stackStart - 1);
  uint64_t pop = threadContext->stack.pop<1>();
  EXPECT_EQ(float_cast<float>(pop), 0.0);
  EXPECT_EQ(threadContext->stack.sp, stackStart);

  instructions[0] = Instructions::fconst_1;
  threadContext->pc = instructions;
  threadContext->callNext();
  EXPECT_EQ(threadContext->pc, instructions + 1);
  EXPECT_EQ(threadContext->stack.sp, (uint32_t *)stackStart - 1);
  pop = threadContext->stack.pop<1>();
  EXPECT_EQ(float_cast<float>(pop), 1.0);
  EXPECT_EQ(threadContext->stack.sp, stackStart);

  instructions[0] = Instructions::fconst_2;
  threadContext->pc = instructions;
  threadContext->callNext();
  EXPECT_EQ(threadContext->pc, instructions + 1);
  EXPECT_EQ(threadContext->stack.sp, (uint32_t *)stackStart - 1);
  pop = threadContext->stack.pop<1>();
  EXPECT_EQ(float_cast<float>(pop), 2.0);
  EXPECT_EQ(threadContext->stack.sp, stackStart);
}

TEST_F(Ins, Dconst) {
  uint8_t instructions[] = {Instructions::dconst_0, 0};
  threadContext->pc = instructions;
  threadContext->callNext();
  EXPECT_EQ(threadContext->pc, instructions + 1);
  EXPECT_EQ(threadContext->stack.sp, (uint64_t *)stackStart - 1);
  uint64_t pop = threadContext->stack.pop<2>();
  EXPECT_EQ(float_cast<double>(pop), 0.0);
  EXPECT_EQ(threadContext->stack.sp, stackStart);

  instructions[0] = Instructions::dconst_1;
  threadContext->pc = instructions;
  threadContext->callNext();
  EXPECT_EQ(threadContext->pc, instructions + 1);
  EXPECT_EQ(threadContext->stack.sp, (uint64_t *)stackStart - 1);
  pop = threadContext->stack.pop<2>();
  EXPECT_EQ(float_cast<double>(pop), 1.0);
  EXPECT_EQ(threadContext->stack.sp, stackStart);
}
