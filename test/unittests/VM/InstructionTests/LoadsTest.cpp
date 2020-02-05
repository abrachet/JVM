
#include "JVM/VM/Instructions.h"
#include "JVM/VM/Stack.h"
#include "JVM/VM/ThreadContext.h"
#include "gtest/gtest.h"

struct Loads : public ::testing::Test {
  ThreadContext tc;

  Loads() : tc(std::move(*Stack::createStack(0x1000))) {
    using namespace std::literals;
    tc.pushFrame("General"sv);
  }

  void SetUp() override {
    if (ClassLoader::classPath.size() < 2) {
      std::string rtJar;
      ASSERT_FALSE(findRTJar(rtJar).size());
      ASSERT_TRUE(rtJar.size());
      ClassLoader::classPath.push_back(rtJar);
    }
    auto classOrError = ClassLoader::loadClass(tc.getCurrentClassName());
    ASSERT_TRUE(classOrError) << classOrError.getError();
  }

  void setUpMethod(int methodIndex) {
    auto classOrError = ClassLoader::loadClass(tc.getCurrentClassName());
    ASSERT_TRUE(classOrError) << classOrError.getError();
    auto &classFile = classOrError.get().second.loadedClass;
    auto &methods = classFile->getMethods();
    ASSERT_EQ(methods[methodIndex].attributeCount, 1);
    int attrNameIdx = methods[methodIndex].attributes[0].attributeNameIndex;
    auto &utf8 =
        classFile->getConstPool().get<Class::ConstPool::Utf8Info>(attrNameIdx);
    ASSERT_EQ(std::string("Code"), std::string(utf8));
    using Class::CodeAttribute;
    CodeAttribute ca =
        CodeAttribute::fromAttr(methods[methodIndex].attributes[0]);
    tc.pc = ca.code;
    ASSERT_TRUE(tc.pc);
  }

public:
  const uint8_t *getCode() const {
    return reinterpret_cast<const uint8_t *>(tc.pc);
  }

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
  tc.callNext();
  tc.callNext();
  tc.callNext();
  tc.callNext(); // Invocation of returnArg here.
  EXPECT_EQ(returnArgGot, 5);
  tc.callNext();                    // Store in 1
  uint64_t got = tc.stack.pop<1>(); // Pop off 1
  EXPECT_EQ(got, 5);
  got = tc.stack.pop<1>(); // Now pop off the local in 0
  EXPECT_EQ(got, 5);
}
