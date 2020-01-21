
#include "JVM/Class/ClassFile.h"
#include "JVM/Class/ClassLoader.h"
#include "JVM/Core/BigEndianByteReader.h"
#include "JVM/Core/ErrorOr.h"
#include "JVM/VM/JNI.h"
#include "JVM/VM/ThreadContext.h"
#include "JVM/VM/TypeReader.h"
#include "JVM/string_view"
#include <algorithm>
#include <cassert>
#include <string>
#include <vector>

static std::string getSymbolName(const std::string &className,
                                 const Class::ConstPool &constPool,
                                 const Class::Method &method) {
  // JNI symbols look like Java_<className>_<method_name>
  std::string sym("Java_");
  sym += className;
  sym += "_";
  auto &utf8 = constPool.get<Class::ConstPool::Utf8Info>(method.nameIndex);
  sym += static_cast<std::string>(utf8);
  return sym;
}

static std::vector<uint64_t> popMethodArgs(ThreadContext &tc,
                                           FuncOrSingleType functionType) {
  assert(functionType.first.first == Function && "Type was not a function");
  auto popArg = [&tc](Type t) -> uint64_t {
    assert(t != Function && t != Void && "Invalid method type");
    if (t.getStackEntryCount() == 1)
      return tc.stack.pop<1>();
    assert(t.getStackEntryCount() == 2);
    return tc.stack.pop<2>();
  };
  std::vector<uint64_t> args;
  // Need to skip the first type which is the return type.
  std::transform(functionType.second.begin() + 1, functionType.second.end(),
                 std::back_inserter(args),
                 [&popArg](auto type) { return popArg(type.first); });
  return args;
}

static void callNative(ThreadContext &tc, const Class::ConstPool &constPool,
                       const Class::Method &method) {
  std::string sym = getSymbolName(tc.loadedClassName, constPool, method);
  void *handle = getMethodHandle(sym.c_str());
  auto &utf8 =
      constPool.get<Class::ConstPool::Utf8Info>(method.descriptorIndex);
  ErrorOr<FuncOrSingleType> typeOrErr =
      parseType(static_cast<std::string_view>(utf8));
  assert(typeOrErr && "Invalid type string");
  std::vector<uint64_t> args = popMethodArgs(tc, *typeOrErr);
  uint64_t retVal = invoke(handle, args);
  size_t size = typeOrErr->second[0].first.getStackEntryCount();
  if (size == 1)
    tc.stack.push<1>(retVal);
  else if (size == 2)
    tc.stack.push<2>(retVal);
}

void invokestatic(ThreadContext &tc) {
  auto &[lock, loadedClass] = tc.getLoadedClass();
  auto &classFile = loadedClass.loadedClass;
  uint16_t cpIndex = readFromPointer<uint16_t>(tc.pc);
  auto &methodRef =
      classFile->getConstPool().get<Class::ConstPool::MethodrefInfo>(cpIndex);
  ErrorOr<const Class::Method &> methodOrErr =
      classFile->findStaticMethod(methodRef);
  assert(methodOrErr && "method not found");
  const Class::Method &method = *methodOrErr;
  if (method.accessFlags & Class::Method::Native)
    return callNative(tc, classFile->getConstPool(), method);
}
