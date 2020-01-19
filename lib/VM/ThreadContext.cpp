
#include "JVM/VM/ThreadContext.h"
#include "JVM/Core/BigEndianByteReader.h"
#include "JVM/VM/Instructions.h"

void ThreadContext::callNext() {
  assert(pc && "pc is nullptr");
  uint8_t ins = readFromPointer<uint8_t>(pc);
  return instructions[ins](*this);
}
