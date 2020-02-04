
#include "JVM/VM/ThreadContext.h"
#include "gtest/gtest.h"

TEST(ThreadContext, LoadLocalVariable) {
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

TEST(ThreadContext, StoreLocalVariable) {
  auto stackOrErr = Stack::createStack(4096);
  ASSERT_TRUE(stackOrErr);
  ThreadContext tc(std::move(*stackOrErr));
  tc.pushFrame("<test>");
  tc.stack.push<1>(10);
  uint64_t got = tc.stack.pop<1>();
  ASSERT_EQ(got, 10);
  tc.stack.push<1>(56); // Make room for local.
  tc.storeInLocal<1>(0, got);
  got = tc.stack.pop<1>();
  ASSERT_EQ(got, 10);
}