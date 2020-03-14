// Copyright 2020 Alex Brachet
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "JVM/Class/ClassFile.h"
#include "JVM/Core/BigEndianByteReader.h"
#include "JVM/Core/ErrorOr.h"
#include "JVM/Core/Iterator.h"
#include "JVM/Core/algorithm.h"
#include "JVM/VM/Allocator.h"
#include "JVM/VM/ClassLoader.h"
#include "JVM/VM/JNI.h"
#include "JVM/VM/ObjectRepresentation.h"
#include "JVM/VM/ThreadContext.h"
#include "JVM/VM/Type.h"
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

  mutable const ClassFile *methodClassFile = nullptr;
  mutable const Class::Method *method = nullptr;

  const ClassFile &getMethodClassFile() const {
    if (methodClassFile)
      return *methodClassFile;
    ErrorOr<LoadedClass &> loadedClass =
        ClassLoader::loadClass(methodClassName);
    assert(loadedClass && "ClassNotFoundException");
    methodClassFile = loadedClass->second->loadedClass.get();
    return *methodClassFile;
  }

  const Class::Method &getMethod() const {
    if (method)
      return *method;
    ErrorOr<const Class::Method &> methodOrErr =
        getMethodClassFile().findStaticMethod(tc.getClassFile(), methodRef);
    // TODO: better error handling here.
    assert(methodOrErr && "MethodNotFoundException");
    method = std::addressof(*methodOrErr);
    return *method;
  }

  const Type &getType() const { return functionType; }

  const Class::ConstPool &getConstPool() const {
    return tc.getClassFile().getConstPool();
  }

  // TODO: Create JavaEnv structure.
  uint64_t getJavaEnv() const { return 0; }

  std::vector<uint64_t> popMethodArgs();

  std::string getNativeSymbol() const {
    std::string sym = "Java_";
    std::string className(methodClassName);
    jvm::replace(className, '/', '_');
    return sym + className + "_" + std::string(functionName);
  }

  void pushFrame() {
    tc.pushFrame(methodClassName);
    if (isSynchronizedMethod())
      tc.currentFrame().syncronizedMethod = true;
  }

  Frame popFrame() { return tc.popFrame(); }

  size_t getArgSize() const {
    return jvm::accumulate(getType().getFunctionArgs(), 0,
                           [](int current, const auto &type) {
                             return current + type.getStackEntryCount();
                           });
  }

  void *findFrameStart() const {
    return reinterpret_cast<uint32_t *>(tc.stack.sp) + getArgSize();
  }

  bool isSynchronizedMethod() const {
    return getMethod().accessFlags & Class::Method::AccessFlags::Synchronized;
  }

  void aquireClassMonitor() {
    ErrorOr<LoadedClass &> loadedClassOrErr =
        ClassLoader::loadClass(methodClassName);
    assert(loadedClassOrErr);
    LoadedClass &loadedClass = *loadedClassOrErr;
    loadedClass.second->monitor.lock();
  }

  void invokeNative();
  void invokeJVM();

  FunctionCaller(ThreadContext &tc, const MethodrefInfo &methodRef,
                 Type functionType)
      : tc(tc), methodRef(methodRef), functionType(functionType) {}

public:
  static ErrorOr<FunctionCaller> create(ThreadContext &tc,
                                        uint16_t methodRefIndex);

  bool isStaticMethod() const {
    return getMethod().accessFlags & Class::Method::AccessFlags::Static;
  }

  bool isNativeMethod() const {
    return getMethod().accessFlags & Class::Method::AccessFlags::Native;
  }

  void call() {
    if (isStaticMethod() && isSynchronizedMethod())
      aquireClassMonitor();
    pushFrame();
    isNativeMethod() ? invokeNative() : invokeJVM();
  }
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
  auto it = jvm::transform(params, jvm::inserter(vec, vec.rbegin()),
                           [this](auto type) {
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
  popFrame();
}

void FunctionCaller::invokeJVM() {
  assert(!isNativeMethod());
  const Class::Method &method = getMethod();
  ErrorOr<Class::CodeAttribute> codeOrErr =
      method.findCodeAttr(getMethodClassFile().getConstPool());
  assert(codeOrErr);
  const Class::CodeAttribute &attr = *codeOrErr;
  tc.currentFrame().frameStart = findFrameStart();
  const auto &cp = getMethodClassFile().getConstPool();
  const auto &nameType =
      cp.get<Class::ConstPool::NameAndTypeInfo>(methodRef.nameAndTypeIndex);
  tc.currentFrame().nameIndex = nameType.nameIndex;
  tc.currentFrame().typeIndex = nameType.descriptorIndex;
  assert(tc.currentFrame().className == methodClassName);
  int extraLocals = attr.maxLocals - getArgSize();
  for (int i = 0; i < extraLocals; i++)
    tc.stack.push<1>(0xDEAD);
  tc.pc = attr.code;
}

void invokestatic(ThreadContext &tc) {
  uint16_t cpIndex = readFromPointer<uint16_t>(tc.pc);
  ErrorOr<FunctionCaller> caller = FunctionCaller::create(tc, cpIndex);
  assert(caller && "throw here");
  assert(caller->isStaticMethod());
  caller->call();
}

void new_(ThreadContext &tc) {
  using Utf8Info = Class::ConstPool::Utf8Info;
  using ClassInfo = Class::ConstPool::ClassInfo;
  uint16_t cpIndex = readFromPointer<uint16_t>(tc.pc);
  const auto &cp = tc.getClassFile().getConstPool();
  const ClassInfo &ci = cp.get<ClassInfo>(cpIndex);
  ErrorOr<LoadedClass &> loadedClass =
      *ClassLoader::loadClass(cp.get<Utf8Info>(ci.nameIndex));
  assert(loadedClass && "Error loading class");
  uint32_t objRef = jvm::allocate(*loadedClass->second);
  tc.stack.push<1>(objRef);
}
