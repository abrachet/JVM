
#include "JVM/Class/ClassLoader.h"
#include "JVM/VM/Instructions.h"
#include "JVM/VM/Stack.h"
#include "JVM/VM/ThreadContext.h"
#include "JVM/string_view"
#include "gtest/gtest.h"

template <size_t StackSize = 0x1000>
struct MethodCaller : public ::testing::Test {
  ThreadContext tc;

protected:
  virtual std::string_view getClassName() = 0;

public:
  MethodCaller() : tc(std::move(*Stack::createStack(StackSize))) {}

  void SetUp() override {
    if (ClassLoader::classPath.size() < 2) {
      std::string rtJar;
      ASSERT_FALSE(findRTJar(rtJar).size());
      ASSERT_TRUE(rtJar.size());
      ClassLoader::classPath.push_back(rtJar);
    }
    auto classOrError = ClassLoader::loadClass(getClassName());
    ASSERT_TRUE(classOrError) << classOrError.getError();
  }

protected:
  void setUpMethod(int methodIndex) {
    auto classOrError = ClassLoader::loadClass(getClassName());
    ASSERT_TRUE(classOrError) << classOrError.getError();
    const auto &classFile = classOrError.get().second.loadedClass;
    const auto &methods = classFile->getMethods();
    const auto &method = methods.at(methodIndex);
    ASSERT_EQ(method.attributeCount, 1);
    int attrNameIdx = method.attributes[0].attributeNameIndex;
    auto &utf8 =
        classFile->getConstPool().template get<Class::ConstPool::Utf8Info>(
            attrNameIdx);
    ASSERT_EQ("Code", std::string_view(utf8));
    using Class::CodeAttribute;
    CodeAttribute ca = CodeAttribute::fromAttr(method.attributes[0]);
    tc.pushFrame(Frame(getClassName(), nullptr, tc.stack.sp, method.nameIndex,
                       method.descriptorIndex));
    tc.pc = ca.code;
    ASSERT_TRUE(tc.pc);
  }

public:
  const uint8_t *getCode() const {
    return reinterpret_cast<const uint8_t *>(tc.pc);
  }

  void callMultiple(int num) {
    for (int i = 0; i < num; i++)
      tc.callNext();
  }
};
