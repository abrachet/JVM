

#include "JVM/VM/JNI.h"
#include "JVM/Class/ClassFinder.h"
#include "JVM/Class/ClassLoader.h"
#include "gtest/gtest.h"
#include <cstdlib>
#include <dlfcn.h>
#include <functional>
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

TEST(JNI, LoadLibjava) {
  // TODO: libjvm is manually loaded here because loading libjava is dependent
  // on libjvm. Add libjvm's path to the rpath of this executable.
  void *handle = loadLibrary(LIBJVM_PATH);
  ASSERT_TRUE(handle);
  handle = loadLibrary(LIBJAVA_PATH);
  ASSERT_TRUE(handle);
  // Test istty because it doesn't need anything from the host jvm.
  void *method = getMethodHandle("Java_java_io_Console_istty");
  ASSERT_TRUE(method);
  using IsttyT = bool(void);
  std::function<IsttyT> istty = reinterpret_cast<IsttyT *>(method);
  bool current = !!::isatty(1);
  EXPECT_EQ(current, istty());
  int newOut = ::dup(1);
  (void)::close(1);
  // Don't test these inline because gtest streams to stdout, not stderr.
  current = !!::isatty(1);
  bool jtty = istty();
  (void)::dup(newOut);
  EXPECT_EQ(current, jtty);
}
