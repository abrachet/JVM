
#include "JVM/Class/ClassFile.h"
#include "JVM/Core/BigEndianByteReader.h"
#include "JVM/string_view"
#include <algorithm>

using namespace Class;

CodeAttribute CodeAttribute::fromAttr(const Attribute &attr) {
  CodeAttribute ca(attr);
  const uint8_t *codePtr = reinterpret_cast<const uint8_t *>(attr.mem);
  readFromPointer(ca.maxStack, codePtr);
  readFromPointer(ca.maxLocals, codePtr);
  readFromPointer(ca.codeLength, codePtr);
  assert(ca.codeLength < attr.attributeLength && "Code length too long");
  assert(ca.codeLength > 0 && ca.codeLength < 65536);
  ca.code = codePtr;
  codePtr += ca.codeLength;
  readFromPointer(ca.exceptionTableLength, codePtr);
  return ca;
}

using MethodrefInfo = Class::ConstPool::MethodrefInfo;

static std::pair<std::string_view, std::string_view>
getClassAndTypeName(const Class::ConstPool &constPool, uint16_t index1,
                    uint16_t index2) {
  auto &utf8 = constPool.get<ConstPool::Utf8Info>(index1);
  std::string_view className = static_cast<std::string_view>(utf8);
  auto &utf82 = constPool.get<ConstPool::Utf8Info>(index2);
  std::string_view nameType = static_cast<std::string_view>(utf82);
  return {className, nameType};
}

ErrorOr<const Class::Method &>
ClassFile::findStaticMethod(const MethodrefInfo &methodInfo) const {
  auto &methods = getMethods();
  auto &constPool = getConstPool();
  // auto &classEntry =
  // constPool.get<ConstPool::ClassInfo>(methodInfo.classIndex);
  auto &nameTypeInfo =
      constPool.get<ConstPool::NameAndTypeInfo>(methodInfo.nameAndTypeIndex);
  auto [className, nameType] = getClassAndTypeName(
      constPool, nameTypeInfo.nameIndex, nameTypeInfo.descriptorIndex);
  auto it = std::find_if(methods.begin(), methods.end(),
                         [&constPool, className = className,
                          nameType = nameType](const Class::Method &method) {
                           auto [otherClassName, otherNameType] =
                               getClassAndTypeName(constPool, method.nameIndex,
                                                   method.descriptorIndex);
                           return otherClassName == className &&
                                  otherNameType == nameType;
                         });
  if (it == methods.end())
    return std::string("Couldn't find method");
  return *it;
}
