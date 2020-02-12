
#ifndef JVM_VM_OBJECTS_H
#define JVM_VM_OBJECTS_H

#include "JVM/Class/ClassFile.h"
#include "JVM/Core/ErrorOr.h"
#include "JVM/VM/Type.h"
#include "JVM/string_view"
#include <vector>

namespace jvm {

using jfieldID = unsigned;

}

class ObjectRepresentation {
  std::vector<std::pair<Type, uint64_t>> memberFields;
  uint64_t size;

  ObjectRepresentation() = default;

public:
  static ErrorOr<ObjectRepresentation> createFromClassFile(const ClassFile &);

  uint64_t getObjectSize() const { return size; }
  uint64_t getFieldOffset(jvm::jfieldID id) const {
    return memberFields[id].second;
  }
};

#endif // JVM_VM_OBJECTS_H
