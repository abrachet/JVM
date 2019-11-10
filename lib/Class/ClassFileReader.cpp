
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
  
  return {std::move(classFile), std::string{}};
}
