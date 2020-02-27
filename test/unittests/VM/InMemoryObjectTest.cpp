
#include "JVM/VM/InMemoryObject.h"
#include "gtest/gtest.h"

TEST(InMemorObject, GetThis) {
  jvm::Class *ptr = nullptr;
  InMemoryObject obj{*ptr};
  // Be careful of pointer provence issues, uintptr comparassions are the safest
  // bet to not be optimized out by the compiler.
  uintptr_t addr = reinterpret_cast<uintptr_t>(std::addressof(obj));
  addr += sizeof(obj);
  EXPECT_EQ(addr, reinterpret_cast<uintptr_t>(obj.getThisptr()));
}
