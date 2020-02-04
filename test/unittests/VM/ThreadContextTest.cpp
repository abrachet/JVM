
#include "JVM/VM/ThreadContext.h"
#include "gtest/gtest.h"

TEST(ThreadContext, LocalVariable) {
  auto stackOrErr = Stack::createStack(4096);
  ASSERT_TRUE(stackOrErr);
  ThreadContext tc(std::move(*stackOrErr));
  tc.pushFrame("<test>");
  tc.stack.push<1>(10);
  tc.stack.push<2>(100);
  uint64_t iload0 = tc.loadFromLocal<1>(0);
  EXPECT_EQ(iload0, 10);
  uint64_t lload1 = tc.loadFromLocal<2>(1);
  EXPECT_EQ(lload1, 100);
}
