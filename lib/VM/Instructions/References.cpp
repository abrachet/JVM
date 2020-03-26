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
#include "JVM/VM/ClassHierarchyWalker.h"
#include "JVM/VM/ClassLoader.h"
#include "JVM/VM/Exceptions.h"
#include "JVM/VM/JNI.h"
#include "JVM/VM/ObjectRepresentation.h"
#include "JVM/VM/ThreadContext.h"
#include "JVM/VM/Type.h"
#include "JVM/string_view"
#include <algorithm>
#include <array>
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
  std::string_view typeName;
  Type functionType;
  bool invokeExplicit = false;

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

  ClassHierarchyWalker::ClassAndMethod getForSpecial();
  Class::CodeAttribute getJVMStaticMethod();
  Class::CodeAttribute getJVMVirtualMethod();
  Class::CodeAttribute getJVMMethod();

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
    size_t size = jvm::accumulate(getType().getFunctionArgs(), 0,
                                  [](int current, const auto &type) {
                                    return current + type.getStackEntryCount();
                                  });
    if (!isStaticMethod())
      size += 1;
    return size;
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

  void makeInvokeExplicit() { invokeExplicit = true; }

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
  caller.typeName = functionType;
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
    *it = tc.stack.pop<1>();
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

Class::CodeAttribute FunctionCaller::getJVMStaticMethod() {
  const Class::Method &method = getMethod();
  const auto &cp = getMethodClassFile().getConstPool();
  ErrorOr<Class::CodeAttribute> codeOrErr = method.findCodeAttr(cp);
  assert(codeOrErr);
  const auto &nameType =
      cp.get<Class::ConstPool::NameAndTypeInfo>(methodRef.nameAndTypeIndex);
  tc.currentFrame().pcStart = codeOrErr->code;
  tc.currentFrame().nameIndex = nameType.nameIndex;
  tc.currentFrame().typeIndex = nameType.descriptorIndex;
  return *codeOrErr;
}

ClassHierarchyWalker::ClassAndMethod FunctionCaller::getForSpecial() {
  ErrorOr<LoadedClass &> clssOrErr = ClassLoader::loadClass(methodClassName);
  assert(clssOrErr);
  ErrorOr<const Class::Method &> methodOrErr =
      clssOrErr->second->loadedClass->findMethodByNameType(functionName,
                                                           typeName);
  assert(methodOrErr);
  return {*clssOrErr->second, *methodOrErr};
}

Class::CodeAttribute FunctionCaller::getJVMVirtualMethod() {
  uint32_t *stackStart = reinterpret_cast<uint32_t *>(findFrameStart());
  auto *ptr =
      reinterpret_cast<InMemoryObject *>(jvm::getAllocatedItem(*stackStart));
  assert(ptr && "NullPointerException");
  ClassHierarchyWalker walker(ptr->clss);
  auto [clss, method] = invokeExplicit
                            ? getForSpecial()
                            : walker.findVirtualMethod(functionName, typeName);
  ErrorOr<Class::CodeAttribute> codeOrErr =
      method.findCodeAttr(clss.loadedClass->getConstPool());
  tc.currentFrame().nameIndex = method.nameIndex;
  tc.currentFrame().typeIndex = method.descriptorIndex;
  return *codeOrErr;
}

Class::CodeAttribute FunctionCaller::getJVMMethod() {
  return isStaticMethod() ? getJVMStaticMethod() : getJVMVirtualMethod();
}

void FunctionCaller::invokeJVM() {
  assert(!isNativeMethod());
  auto attr = getJVMMethod();
  tc.currentFrame().frameStart = findFrameStart();
  assert(tc.currentFrame().className == methodClassName);
  int extraLocals = attr.maxLocals - getArgSize();
  for (int i = 0; i < extraLocals; i++)
    tc.stack.push<1>(0xDEAD);
  tc.pc = attr.code;
}

void invokevirtual(ThreadContext &tc) {
  uint16_t cpIndex = readFromPointer<uint16_t>(tc.pc);
  ErrorOr<FunctionCaller> caller = FunctionCaller::create(tc, cpIndex);
  assert(caller && "throw here");
  assert(!caller->isStaticMethod());
  caller->call();
}

void invokespecial(ThreadContext &tc) {
  uint16_t cpIndex = readFromPointer<uint16_t>(tc.pc);
  ErrorOr<FunctionCaller> caller = FunctionCaller::create(tc, cpIndex);
  assert(caller && "throw here");
  assert(!caller->isStaticMethod());
  caller->makeInvokeExplicit();
  caller->call();
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

using FieldrefInfo = Class::ConstPool::FieldrefInfo;
using NameAndTypeInfo = Class::ConstPool::NameAndTypeInfo;
using Utf8Info = Class::ConstPool::Utf8Info;

static Type getFieldType(const ClassFile &cf, const FieldrefInfo &fieldRef) {
  uint16_t nameTypeIndex = fieldRef.nameAndTypeIndex;
  const auto &nt = cf.getConstPool().get<NameAndTypeInfo>(nameTypeIndex);
  std::string_view type = cf.getConstPool().get<Utf8Info>(nt.descriptorIndex);
  ErrorOr<Type> typeOrErr = Type::parseType(type);
  assert(typeOrErr && "couldn't parse type");
  return *typeOrErr;
}

static uint32_t getFieldID(const ClassFile &cf, const FieldrefInfo &fieldRef) {
  const auto &constPool = cf.getConstPool();
  const auto &fields = cf.getFields();
  const auto &nameType =
      constPool.get<NameAndTypeInfo>(fieldRef.nameAndTypeIndex);
  auto it = jvm::find_if(fields, [&](const auto &field) {
    return nameType.nameIndex == field.nameIndex &&
           nameType.descriptorIndex == field.descriptorIndex;
  });
  assert(it != fields.end());
  return std::distance(fields.begin(), it);
}

static std::pair<void *, size_t> getField(const ClassFile &cf, uint32_t cpIndex,
                                          uint32_t objectKey) {
  const auto &fieldRef = cf.getConstPool().get<FieldrefInfo>(cpIndex);

  Type fieldType = getFieldType(cf, fieldRef);
  uint32_t fieldID = getFieldID(cf, fieldRef);
  InMemoryItem *object = jvm::getAllocatedItem(objectKey);
  assert(object);
  const auto &objectRep =
      reinterpret_cast<InMemoryObject *>(object)->getObjectRepresentation();
  return {reinterpret_cast<char *>(object->getThisptr()) +
              objectRep.getFieldOffset(fieldID),
          fieldType.getStackEntryCount()};
}

template <size_t StackSize>
static void pushFromAddr(ThreadContext &tc, const void *ptr) {
  const auto *p = reinterpret_cast<const Stack::EntryType<StackSize> *>(ptr);
  tc.stack.push<StackSize>(*p);
}

void getfield(ThreadContext &tc) {
  uint16_t cpIndex = readFromPointer<uint16_t>(tc.pc);
  uint32_t objectKey = tc.stack.pop<1>();
  auto pair = getField(tc.getClassFile(), cpIndex, objectKey);

  if (pair.second == 1)
    return pushFromAddr<1>(tc, pair.first);
  assert(pair.second == 2 && "invalid stack size");
  return pushFromAddr<2>(tc, pair.first);
}

template <size_t StackSize>
static void writeToAddr(void *ptr, uint64_t toWrite) {
  auto *p = reinterpret_cast<Stack::EntryType<StackSize> *>(ptr);
  *p = static_cast<Stack::EntryType<StackSize>>(toWrite);
}

void putfield(ThreadContext &tc) {
  uint16_t cpIndex = readFromPointer<uint16_t>(tc.pc);
  const auto &fieldRef =
      tc.getClassFile().getConstPool().get<FieldrefInfo>(cpIndex);
  Type fieldType = getFieldType(tc.getClassFile(), fieldRef);

  uint64_t objectOnStack;
  if (fieldType.getStackEntryCount() == 1)
    objectOnStack = tc.stack.pop<1>();
  else {
    assert(fieldType.getStackEntryCount() == 1);
    objectOnStack = tc.stack.pop<2>();
  }

  uint32_t objectKey = tc.stack.pop<1>();
  auto pair = getField(tc.getClassFile(), cpIndex, objectKey);

  if (pair.second == 1)
    return writeToAddr<1>(pair.first, objectOnStack);
  assert(pair.second == 2 && "invalid stack size");
  return writeToAddr<2>(pair.first, objectOnStack);
}

static std::array<Type, 12> integralTypes{
    Type(Invalid), Type(Invalid), Type(Invalid), Type(Invalid),
    Type(Boolean), Type(Char),    Type(Float),   Type(Double),
    Type(Byte),    Type(Short),   Type(Int),     Type(Long)};

void newarray(ThreadContext &tc) {
  uint8_t type = readFromPointer<uint8_t>(tc.pc);
  assert(type < integralTypes.size());
  Type &t = integralTypes[type];
  assert(t != Invalid);
  uint32_t count = tc.stack.pop<1>();
  assert(count >= 0 && "NegativeArraySizeException");
  uint32_t key = jvm::allocateArray(t, count);
  tc.stack.push<1>(key);
}

void arraylength(ThreadContext &tc) {
  uint32_t key = tc.stack.pop<1>();
  InMemoryItem *item = jvm::getAllocatedItem(key);
  tc.stack.push<1>(reinterpret_cast<InMemoryArray *>(item)->length);
}

void athrow(ThreadContext &tc) {
  uint32_t key = tc.stack.pop<1>();
  unwindFromException(tc, key);
}

void instanceof (ThreadContext & tc) {
  using ClassInfo = Class::ConstPool::ClassInfo;
  uint32_t key = tc.stack.pop<1>();
  uint16_t cpIndex = readFromPointer<uint16_t>(tc.pc);
  const auto &cp = tc.getClassFile().getConstPool();
  const auto &classInfo = cp.get<ClassInfo>(cpIndex);
  std::string_view className = cp.get<Utf8Info>(classInfo.nameIndex);
  InMemoryItem *item = jvm::getAllocatedItem(key);
  assert(item && "NullPointerException");
  if (item->isArray() && className == "java/lang/Object") {
    tc.stack.push<1>(1);
    return;
  }
  auto &obj = reinterpret_cast<InMemoryObject &>(*item);
  ClassHierarchyWalker walker(obj.clss);
  tc.stack.push<1>(walker.extends(className));
}
