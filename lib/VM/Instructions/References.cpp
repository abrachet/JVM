
#include "JVM/Class/ClassFile.h"
#include "JVM/Class/ClassLoader.h"
#include "JVM/Core/BigEndianByteReader.h"
#include "JVM/Core/ErrorOr.h"
#include "JVM/Core/Iterator.h"
#include "JVM/VM/JNI.h"
#include "JVM/VM/ThreadContext.h"
#include "JVM/VM/TypeReader.h"
#include "JVM/string_view"
#include <algorithm>
#include <cassert>
#include <string>
#include <vector>

// TODO: Some asserts need to throw InvalidClassException.

class FunctionCaller {
  using MethodrefInfo = Class::ConstPool::MethodrefInfo;

  const ClassFile &classFile;
  const Class::Method &method;
  const MethodrefInfo &methodRef;
  ThreadContext &tc;

  Type functionType;

  FunctionCaller(const ClassFile &classFile, const Class::Method &method,
                 const MethodrefInfo &methodRef, ThreadContext &tc,
                 Type functionType)
      : classFile(classFile), method(method), methodRef(methodRef), tc(tc),
        functionType(functionType) {}

  const auto &getConstPool() const { return classFile.getConstPool(); }

  // TODO: Create JavaEnv structure.
  uint64_t getJavaEnv() const { return 0; }

  std::vector<uint64_t> popMethodArgs();

  bool isNativeMethod() const {
    return method.accessFlags & Class::Method::AccessFlags::Native;
  }

  std::string getNativeSymbol() const {
    std::string sym("Java_");
    sym += tc.loadedClassName + "_";
    auto &utf8 =
        getConstPool().get<Class::ConstPool::Utf8Info>(method.nameIndex);
    return sym + static_cast<std::string>(utf8);
  }

  void invokeNative();
  // TODO
  void invokeJVM() {}

public:
  static ErrorOr<FunctionCaller> create(ThreadContext &tc,
                                        const ClassFile &classFile,
                                        uint16_t methodRefIndex) {
    auto &cp = classFile.getConstPool();
    const auto *methodRef = cp.get<MethodrefInfo *>(methodRefIndex);
    if (!methodRef)
      return std::string("Index does not point to a method");
    auto methodOrErr = classFile.findStaticMethod(*methodRef);
    if (!methodOrErr)
      return methodOrErr.getError();

    const auto *utf8 =
        cp.get<Class::ConstPool::Utf8Info *>(methodOrErr->descriptorIndex);
    if (!utf8)
      return std::string("Invalid method descriptor index");
    auto typeOrErr = Type::parseType(static_cast<std::string_view>(*utf8));
    if (!typeOrErr)
      return typeOrErr.getError();

    return FunctionCaller(classFile, *methodOrErr, *methodRef, tc, *typeOrErr);
  }

  bool isStaticMethod() const {
    return method.accessFlags & Class::Method::AccessFlags::Static;
  }

  void call() {
    if (isNativeMethod())
      return invokeNative();
    invokeJVM();
  }
};

std::vector<uint64_t> FunctionCaller::popMethodArgs() {
  std::vector<uint64_t> vec{getJavaEnv()};
  const auto &params = functionType.getFunctionArgs();
  vec.resize(params.size() + !isStaticMethod() + 1);
  auto it = std::transform(params.begin(), params.end(),
                           jvm::inserter(vec, vec.rbegin()), [this](auto type) {
                             assert(type.c != Function);
                             if (type.getStackEntryCount() == 1)
                               return tc.stack.pop<1>();
                             assert(type.getStackEntryCount() == 2);
                             return tc.stack.pop<2>();
                           });
  if (!isStaticMethod())
    *it = tc.stack.pop<2>();
  return vec;
}

void FunctionCaller::invokeNative() {
  assert(isNativeMethod());
  std::string sym = getNativeSymbol();
  void *handle = getMethodHandle(sym.c_str());
  assert(handle && "UnsatisfiedLinkError");
  std::vector<uint64_t> args = popMethodArgs();
  uint64_t ret = invoke(handle, args);
  size_t size = functionType.getReturnType().getStackEntryCount();
  if (size == 2)
    tc.stack.push<2>(ret);
  else if (size == 1)
    tc.stack.push<1>(ret);
  else
    assert(size == 0 && "Invalid stack size for return type");
}

void invokestatic(ThreadContext &tc) {
  auto &[lock, loadedClass] = tc.getLoadedClass();
  const auto &classFile = loadedClass.loadedClass;
  uint16_t cpIndex = readFromPointer<uint16_t>(tc.pc);
  ErrorOr<FunctionCaller> caller =
      FunctionCaller::create(tc, *classFile, cpIndex);
  assert(caller && "throw here");
  assert(caller->isStaticMethod());
  caller->call();
}
