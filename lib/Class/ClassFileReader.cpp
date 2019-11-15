
#include "JVM/Class/ClassFileReader.h"
#include <functional>

ClassFileReader::ClassFileOrError ClassFileReader::read() {
  auto classFile = std::make_unique<ClassFile>();
  uint32_t magic = 0;
  if (!reader->read(magic))
    return {nullptr, "reader error"};
  if (magic != 0xCAFEBABE)
    return {nullptr, "incorrect file format"};
  if (!reader->read(classFile->minorVersion))
    return {nullptr, "reader error"};
  if (!reader->read(classFile->majorVersion))
    return {nullptr, "reader error"};

  if (std::string s = readConstPool(*classFile); s != "")
    return {nullptr, s};

  if (!reader->read(classFile->accessFlags))
    return {nullptr, "reader error"};
  if (!reader->read(classFile->thisClass))
    return {nullptr, "reader error"};
  if (!reader->read(classFile->superClass))
    return {nullptr, "reader error"};

  return {std::move(classFile), std::string{}};
}

using namespace Class;

std::string ClassFileReader::readConstPool(ClassFile &classFile) {
  using readCPFunc = std::string (ClassFileReader::*)(ClassFile &);
  static readCPFunc readCPFunctions[ConstPool::Last] = {
      [ConstPool::Utf8] = &ClassFileReader::readCPUtf8,
      [ConstPool::Integer] =
          &ClassFileReader::readCPIntegeral<int32_t, ConstPool::Integer>,
      [ConstPool::Float] =
          &ClassFileReader::readCPIntegeral<decimal32, ConstPool::Float>,
      [ConstPool::Long] =
          &ClassFileReader::readCPIntegeral<int64_t, ConstPool::Long>,
      [ConstPool::Double] =
          &ClassFileReader::readCPIntegeral<decimal64, ConstPool::Double>,
      [ConstPool::Class] = &ClassFileReader::readCPClass,
      [ConstPool::String] = &ClassFileReader::readCPString,
      [ConstPool::Fieldref] = &ClassFileReader::readCPRef<ConstPool::Fieldref>,
      [ConstPool::Methodref] =
          &ClassFileReader::readCPRef<ConstPool::Methodref>,
      [ConstPool::InterfaceMethodref] =
          &ClassFileReader::readCPRef<ConstPool::InterfaceMethodref>,
      [ConstPool::NameAndType] = &ClassFileReader::readCPNameType,
      [ConstPool::MethodHandle] = &ClassFileReader::readCPMethodHandle,
      [ConstPool::MethodType] = &ClassFileReader::readCPMethodType,
      [ConstPool::InvokeDynamic] = &ClassFileReader::readCPInvokeDyn};

  uint16_t count;
  if (!reader->read(count))
    return "reader error";
  classFile.constPool.entries.reserve(count);
  // Pool is 1 indexed so add an emtpy entry at the front.
  classFile.constPool.entries.emplace_back(nullptr);
  for (int i = 0; i < count - 1; i++) {
    uint8_t tag;
    if (!reader->read(tag))
      return "reader error";
    if (tag >= ConstPool::Last)
      return "unkown tag is larger than any known tags";

    readCPFunc method = readCPFunctions[tag];
    if (!method)
      return "unkown tag";
    (this->*method)(classFile);

    if (tag == ConstPool::Long || tag == ConstPool::Double) {
      classFile.constPool.entries.emplace_back(nullptr);
      i++;
    }
  }
  return "";
}

std::string ClassFileReader::readCPUtf8(ClassFile &classFile) {
  ConstPool::Utf8Info utf8;
  if (!reader->read(utf8.length))
    return "reader error";
  utf8.bytes =
      reinterpret_cast<const uint8_t *>(reader->data() + reader->getPos());
  reader->seek(reader->getPos() + utf8.length);
  classFile.constPool.entries.emplace_back(
      std::make_unique<ConstPool::Utf8Info>(utf8));
  return "";
}

template <typename IntT, ConstPool::Type CpType>
std::string ClassFileReader::readCPIntegeral(ClassFile &classFile) {
  ConstPool::IntegralInfo<IntT, CpType> entry;
  if (!reader->read(entry.bytes))
    return "reader error";
  classFile.constPool.entries.emplace_back(
      std::make_unique<decltype(entry)>(entry));
  return "";
}

std::string ClassFileReader::readCPClass(ClassFile &classFile) {
  ConstPool::ClassInfo clazz;
  if (!reader->read(clazz.nameIndex))
    return "reader error";
  classFile.constPool.entries.emplace_back(
      std::make_unique<decltype(clazz)>(clazz));
  return "";
}

std::string ClassFileReader::readCPString(ClassFile &classFile) {
  ConstPool::StringInfo string;
  if (!reader->read(string.stringIndex))
    return "reader error";
  classFile.constPool.entries.emplace_back(
      std::make_unique<decltype(string)>(string));
  return "";
}

template <Class::ConstPool::Type CpType>
std::string ClassFileReader::readCPRef(ClassFile &classFile) {
  ConstPool::RefInfo<CpType> refInfo;
  if (!reader->read(refInfo.classIndex))
    return "reader error";
  if (!reader->read(refInfo.nameAndTypeIndex))
    return "reader error";
  classFile.constPool.entries.emplace_back(
      std::make_unique<decltype(refInfo)>(refInfo));
  return "";
}

std::string ClassFileReader::readCPNameType(ClassFile &classFile) {
  ConstPool::NameAndTypeInfo nameType;
  if (!reader->read(nameType.nameIndex))
    return "reader error";
  if (!reader->read(nameType.descriptorIndex))
    return "reader error";
  classFile.constPool.entries.emplace_back(
      std::make_unique<decltype(nameType)>(nameType));
  return "";
}

std::string ClassFileReader::readCPMethodHandle(ClassFile &classFile) {
  ConstPool::MethodHandleInfo method;
  if (!reader->read(method.referenceKind))
    return "reader error";
  if (!reader->read(method.referenceIndex))
    return "reader error";
  classFile.constPool.entries.emplace_back(
      std::make_unique<decltype(method)>(method));
  return "";
}

std::string ClassFileReader::readCPMethodType(ClassFile &classFile) {
  ConstPool::MethodTypeInfo method;
  if (!reader->read(method.descriptorIndex))
    return "reader error";
  classFile.constPool.entries.emplace_back(
      std::make_unique<decltype(method)>(method));
  return "";
}

std::string ClassFileReader::readCPInvokeDyn(ClassFile &classFile) {
  ConstPool::InvokeDynamicInfo invD;
  if (!reader->read(invD.bootstrapMethodAttrIndex))
    return "reader error";
  if (!reader->read(invD.nameAndTypeIndex))
    return "reader error";
  classFile.constPool.entries.emplace_back(
      std::make_unique<decltype(invD)>(invD));
  return "";
}
