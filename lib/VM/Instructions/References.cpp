
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

class FunctionCaller {
  using MethodrefInfo = Class::ConstPool::MethodrefInfo;
  using Utf8Info = Class::ConstPool::Utf8Info;
  using ClassInfo = Class::ConstPool::ClassInfo;
  using NameAndTypeInfo = Class::ConstPool::NameAndTypeInfo;

  ThreadContext &tc;
  const MethodrefInfo &methodRef;
  std::string_view methodClassName;
  std::string_view functionName;
  Type functionType;

  const ClassFile *methodClassFile = nullptr;
  const Class::Method *method = nullptr;

  const ClassFile &getMethodClassFile() {
    if (methodClassFile)
      return *methodClassFile;
    ErrorOr<ClassLoader::LoadedClass &> loadedClass =
        ClassLoader::loadClass(methodClassName);
    assert(loadedClass && "ClassNotFoundException");
    methodClassFile = loadedClass->second.loadedClass.get();
    return *methodClassFile;
  }

  const Class::Method &getMethod() {
    if (method)
      return *method;
    ErrorOr<const Class::Method &> methodOrErr =
        getMethodClassFile().findStaticMethod(tc.getClassFile(), methodRef);
    // TODO: better error handling here.
    assert(methodOrErr && "MethodNotFoundException");
    method = std::addressof(*methodOrErr);
    return *method;
  }

  const Class::ConstPool &getConstPool() const {
    return tc.getClassFile().getConstPool();
  }

  // TODO: Create JavaEnv structure.
  uint64_t getJavaEnv() const { return 0; }

  std::vector<uint64_t> popMethodArgs();

  std::string getNativeSymbol() const {
    std::string sym = "Java_";
    std::string className(methodClassName);
    std::replace(className.begin(), className.end(), '/', '_');
    return sym + className + "_" + std::string(functionName);
  }

  void invokeNative();
  // TODO.
  void invokeJVM() {}

  FunctionCaller(ThreadContext &tc, const MethodrefInfo &methodRef,
                 Type functionType)
      : tc(tc), methodRef(methodRef), functionType(functionType) {}

public:
  static ErrorOr<FunctionCaller> create(ThreadContext &tc,
                                        uint16_t methodRefIndex);

  bool isStaticMethod() {
    return getMethod().accessFlags & Class::Method::AccessFlags::Static;
  }

  bool isNativeMethod() {
    return getMethod().accessFlags & Class::Method::AccessFlags::Native;
  }

  // TODO aquire locks if method is synchronized.
  void call() { isNativeMethod() ? invokeNative() : invokeJVM(); }
};

ErrorOr<FunctionCaller> FunctionCaller::create(ThreadContext &tc,
                                               uint16_t methodRefIndex) {
  const auto &cp = tc.getClassFile().getConstPool();
  const auto &methodRef = cp.get<MethodrefInfo>(methodRefIndex);
  const auto &methodClass = cp.get<ClassInfo>(methodRef.classIndex);
  const auto &nameType = cp.get<NameAndTypeInfo>(methodRef.nameAndTypeIndex);
  std::string_view functionType = cp.get<Utf8Info>(nameType.descriptorIndex);
  ErrorOr<Type> typeOrErr = Type::parseType(functionType);
  if (!typeOrErr)
    return typeOrErr.getError();
  FunctionCaller caller(tc, methodRef, *typeOrErr);
  caller.methodClassName = cp.get<Utf8Info>(methodClass.nameIndex);
  caller.functionName = cp.get<Utf8Info>(nameType.nameIndex);

  return caller;
}

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
  uint16_t cpIndex = readFromPointer<uint16_t>(tc.pc);
  ErrorOr<FunctionCaller> caller = FunctionCaller::create(tc, cpIndex);
  assert(caller && "throw here");
  assert(caller->isStaticMethod());
  caller->call();
}
