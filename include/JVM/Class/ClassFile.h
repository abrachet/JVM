
#ifndef JVM_CLASS_CLASSFILE_H
#define JVM_CLASS_CLASSFILE_H

#include "JVM/Core/FileBuffer.h"
#include <cstdint>
#include <memory>
#include <vector>

class ClassFileReader;

namespace Class {

struct ConstPool;
struct Interfaces;
struct Fields;
struct Methods;
struct Attributes;

struct ConstPool {
  enum Type : uint8_t {
    Utf8 = 1,
    Integer = 3,
    Float = 4,
    Long = 5,
    Double = 6,
    Class = 7,
    String = 8,
    Fieldref = 9,
    Methodref = 10,
    InterfaceMethodref = 11,
    NameAndType = 12,
    MethodHandle = 15,
    MethodType = 16,
    InvokeDynamic = 18,
  };

  struct ConstPoolBase {
    Type tag;
    ConstPoolBase(Type tag) : tag(tag) {}
    virtual ~ConstPoolBase() {}
  };

  struct ClassInfo : public ConstPoolBase {
    ClassInfo(uint16_t nameIndex = 0)
        : ConstPoolBase(Class), nameIndex(nameIndex) {}
    uint16_t nameIndex;
  };

  template <Type t> struct RefInfo : public ConstPoolBase {
    RefInfo(uint16_t classIndex = 0, uint16_t nameAndTypeIndex = 0)
        : ConstPoolBase(t), classIndex(classIndex),
          nameAndTypeIndex(nameAndTypeIndex) {}
    uint16_t classIndex;
    uint16_t nameAndTypeIndex;
  };

  using FieldrefInfo = RefInfo<Fieldref>;
  using MethodrefInfo = RefInfo<Methodref>;
  using InterfaceMethodrefInfo = RefInfo<InterfaceMethodref>;

  struct StringInfo : public ConstPoolBase {
    StringInfo(uint16_t stringIndex = 0)
        : ConstPoolBase(String), stringIndex(stringIndex) {}
    uint16_t stringIndex;
  };

  struct NameAndTypeInfo : public ConstPoolBase {
    NameAndTypeInfo(uint16_t nameIndex = 0, uint16_t descriptorIndex = 0)
        : ConstPoolBase(NameAndType), nameIndex(nameIndex),
          descriptorIndex(descriptorIndex) {}
    uint16_t nameIndex;
    uint16_t descriptorIndex;
  };

  struct Utf8Info : public ConstPoolBase {
    Utf8Info(uint16_t length = 0, const uint8_t *bytes = nullptr)
        : ConstPoolBase(Utf8), length(length), bytes(bytes) {}
    uint16_t length;
    const uint8_t *bytes;
  };

  const std::vector<std::unique_ptr<ConstPoolBase>> &getEntries() const {
    return entries;
  }

private:
  std::vector<std::unique_ptr<ConstPoolBase>> entries;
  friend class ::ClassFileReader;
};

struct Interfaces {};
struct Fields {};
struct Methods {};
struct Attributes {};

} // namespace Class

class ClassFile {
  friend class ClassFileReader;

  uint16_t minorVersion;
  uint16_t majorVersion;

  uint16_t accessFlags;
  uint16_t thisClass;
  uint16_t superClass;

  Class::ConstPool constPool;
  Class::Interfaces interfaces;
  Class::Fields fields;
  Class::Methods methods;
  Class::Attributes attributes;

  std::unique_ptr<FileBuffer> underlyingFile;

public:
  uint16_t getMinorVersion() const { return minorVersion; }
  uint16_t getMajorVersion() const { return majorVersion; }
  const Class::ConstPool &getConstPool() const { return constPool; }
};

#endif // JVM_CLASS_CLASSFILE_H
