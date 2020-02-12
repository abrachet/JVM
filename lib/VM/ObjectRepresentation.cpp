
#include "JVM/VM/ObjectRepresentation.h"
#include "JVM/VM/Stack.h"

ErrorOr<ObjectRepresentation>
ObjectRepresentation::createFromClassFile(const ClassFile &classFile) {
  const auto &constPool = classFile.getConstPool();
  auto getType = [&constPool](int nameIndex) {
    std::string_view sv = constPool.get<Class::ConstPool::Utf8Info>(nameIndex);
    ErrorOr<Type> typeOrErr = Type::parseType(sv);
    assert(typeOrErr && "Couldn't parse type");
    return *typeOrErr;
  };
  constexpr auto oneEntrySize = Stack::stackEntryBytes;
  constexpr auto twoEntrySize = 2 * oneEntrySize;

  ObjectRepresentation OR;
  size_t currentOffset = 0;
  for (const auto &field : classFile.getFields()) {
    if (field.accessFlags & Class::Field::Static)
      continue;
    Type t = getType(field.descriptorIndex);
    size_t fieldSize = t.getStackEntryCount() * oneEntrySize;
    if (currentOffset % fieldSize) {
      currentOffset += oneEntrySize;
      assert(!(currentOffset % twoEntrySize) && "bad alignment");
    }
    OR.memberFields.emplace_back(t, currentOffset);
    currentOffset += fieldSize;
  }
  OR.size = currentOffset;
  return OR;
}
