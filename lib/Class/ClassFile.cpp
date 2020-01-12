
#include "JVM/Class/ClassFile.h"
#include "JVM/Core/BigEndianByteReader.h"

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
