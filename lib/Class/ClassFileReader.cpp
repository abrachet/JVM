
#include "JVM/Class/ClassFileReader.h"

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

  return {std::move(classFile), std::string{}};
}

using namespace Class;

std::string ClassFileReader::readConstPool(ClassFile &classFile) {
  uint16_t count;
  if (!reader->read(count))
    return "reader error";
  classFile.constPool.entries.reserve(count);
  // Pool is 1 indexed.
  classFile.constPool.entries.emplace_back(nullptr);
  for (int i = 0; i < count - 1; i++) {
    uint8_t tag;
    if (!reader->read(tag))
      return "reader error";
    switch (tag) {
    default:
      return "unkown const pool tag";
    case ConstPool::Utf8:
      if (std::string s = readCPUtf8(classFile); s != "")
        return s;
      break;
    case ConstPool::Class:
      if (std::string s = readCPClass(classFile); s != "")
        return s;
      break;
    case ConstPool::String:
      if (std::string s = readCPString(classFile); s != "")
        return s;
      break;
    case ConstPool::Fieldref:
    case ConstPool::Methodref:
    case ConstPool::InterfaceMethodref:
      if (std::string s =
              readCPRef(classFile, static_cast<ConstPool::Type>(tag));
          s != "")
        return s;
      break;
    case ConstPool::NameAndType:
      if (std::string s = readCPNameType(classFile); s != "")
        return s;
      break;
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

std::string ClassFileReader::readCPRef(ClassFile &classFile,
                                       ConstPool::Type tag) {
  ConstPool::RefInfo<ConstPool::Fieldref> refInfo;
  refInfo.tag = tag;
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
