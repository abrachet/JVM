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
#include "JVM/Core/algorithm.h"
#include "JVM/string_view"

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
using NameType = std::pair<std::string_view, std::string_view>;

static NameType getNameType(const Class::ConstPool &constPool, uint16_t index1,
                            uint16_t index2) {
  auto &utf8 = constPool.get<ConstPool::Utf8Info>(index1);
  std::string_view className = static_cast<std::string_view>(utf8);
  auto &utf82 = constPool.get<ConstPool::Utf8Info>(index2);
  std::string_view nameType = static_cast<std::string_view>(utf82);
  return {className, nameType};
}

static ErrorOr<const Class::Method &>
findMethodFromNameType(const NameType &nameType,
                       const Class::ConstPool &constPool,
                       const Methods &methods) {
  auto finder = [&nameType, &constPool](const Class::Method &method) {
    return nameType ==
           getNameType(constPool, method.nameIndex, method.descriptorIndex);
  };
  auto it = jvm::find_if(methods, finder);
  if (it == methods.end())
    return std::string("Method not found");
  return *it;
}

ErrorOr<const Class::Method &>
ClassFile::findStaticMethod(const ClassFile &classFile,
                            const MethodrefInfo &methodInfo) const {
  const auto &otherCP = classFile.getConstPool();
  const auto &ntInfo =
      otherCP.get<ConstPool::NameAndTypeInfo>(methodInfo.nameAndTypeIndex);
  NameType nameType =
      getNameType(otherCP, ntInfo.nameIndex, ntInfo.descriptorIndex);
  return findMethodFromNameType(nameType, getConstPool(), getMethods());
}

std::string_view getMethodName(const Class::ConstPool &constPool,
                               const Class::Method &method) {
  return constPool.get<ConstPool::Utf8Info>(method.nameIndex);
}

ErrorOr<const Class::Method &>
ClassFile::findMethodByNameType(std::string_view name,
                                std::string_view typeName) const {
  return findMethodFromNameType({name, typeName}, getConstPool(), getMethods());
}
