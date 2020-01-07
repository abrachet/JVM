
#include "JVM/Core/Defer.h"
#include "gtest/gtest.h"

TEST(Defer, ScopeExit) {
  bool b = false;
  {
    auto _ = defer([&b] { b = true; });
  }
  EXPECT_TRUE(b);
}
