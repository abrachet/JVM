
#ifndef JVM_CLASS_CLASSFILE_H
#define JVM_CLASS_CLASSFILE_H

#include <cstdint>
#include <memory>
#include "JVM/Core/FileBuffer.h"

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

  struct ClassInfo {
    Type tag = Class;
    uint16_t nameIndex;
  };

  template <Type t>
  struct RefInfo {
    Type tag = t;
    uint16_t classIndex;
    uint16_t nameAndTypeIndex;
  };

  using FieldrefInfo = RefInfo<Fieldref>;
  using MethodrefInfo = RefInfo<Methodref>;
  using InterfaceMethodrefInfo = RefInfo<InterfaceMethodref>;

  struct StringInfo {
    Type tag = String;
    uint16_t stringIndex;
  };

  struct NameAndTypeInfo {
    Type tag = NameAndType;
    uint16_t nameIndex;
    uint16_t descriptorIndex;
  };

  struct Utf8Info {
    Type tag = Utf8;
    uint16_t length;
    uint8_t* bytes;
  };

  struct Info {
    Type tag;
    uint8_t* info;
  };
};

struct Interfaces {};
struct Fields {};
struct Methods {};
struct Attributes {};

}  // namespace Class

class ClassFileReader;

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
};

#endif  // JVM_CLASS_CLASSFILE_H
