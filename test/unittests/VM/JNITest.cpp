

#include "JVM/VM/JNI.h"
#include "JVM/Class/ClassFinder.h"
#include "JVM/Class/ClassLoader.h"
#include "gtest/gtest.h"
#include <cstdlib>
#include <dlfcn.h>
#include <unistd.h>

TEST(JNI, LoadLibrary) {
  void *handle = loadLibrary("ExternSym.so");
  ASSERT_TRUE(handle);
}

TEST(JNI, CallFunction) {
  void *handle = loadLibrary("ExternSym.so");
  ASSERT_TRUE(handle);
  void *method = getMethodHandle("return5");
  ASSERT_TRUE(method);
  ASSERT_EQ(((int (*)())method)(), 5);
}
