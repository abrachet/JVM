
#include "InstructionTests.h"
#include "JVM/VM/ClassLoader.h"
#include "JVM/VM/Instructions.h"
#include "JVM/VM/Stack.h"
#include "JVM/VM/ThreadContext.h"
#include "gtest/gtest.h"
#include <condition_variable>
#include <mutex>
#include <thread>

struct InvokeStaticJVM : public MethodCaller<> {

  std::string_view getClassName() override { return "General"; }

  void setUpCallReturnArgJVM() {
    setUpMethod(4);
    EXPECT_EQ(getCode()[0], Instructions::iconst_1);
    EXPECT_EQ(getCode()[1], Instructions::invokestatic);
    EXPECT_EQ(getCode()[4], Instructions::istore_0);
  }

  void setUpCallSynced() {
    setUpMethod(6);
    EXPECT_EQ(getCode()[0], Instructions::invokestatic);
    EXPECT_EQ(getCode()[3], Instructions::pop);
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

TEST_F(InvokeStaticJVM, Return) {
  setUpCallReturnArgJVM();
  // Need to manually allocate space for local in callReturnArgJVM.
  tc.stack.push<1>(0xDEAD);
  callMultiple(2);
  EXPECT_EQ(getCode()[0], Instructions::iload_0);
  EXPECT_EQ(getCode()[1], Instructions::ireturn);
  ASSERT_EQ(tc.getMethodName(), "returnArgJVM");
  callMultiple(2);
  ASSERT_EQ(tc.getMethodName(), "callReturnArgJVM");
  EXPECT_EQ(tc.stack.pop<1>(), 1);
}

TEST_F(InvokeStaticJVM, Synchronized) {
  setUpCallSynced();
  ASSERT_EQ(tc.getMethodName(), "callSynced");
  tc.callNext();
  EXPECT_EQ(getCode()[0], Instructions::iconst_1);
  EXPECT_EQ(getCode()[1], Instructions::ireturn);
  std::mutex m;
  std::unique_lock lock(m);
  std::condition_variable cv;
  std::thread t1([this, &cv, &m] {
    auto &loadedClass = tc.getLoadedClass();
    ASSERT_FALSE(loadedClass.second.monitor.try_lock());
    cv.notify_one();
    std::unique_lock lock(m);
    cv.wait(lock);
    EXPECT_TRUE(loadedClass.second.monitor.try_lock());
    loadedClass.second.monitor.unlock();
  });
  cv.wait(lock);
  lock.unlock();
  callMultiple(2);
  tc.getLoadedClass().second.monitor.unlock();
  cv.notify_all();
  t1.join();
}
