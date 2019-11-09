
#include "JVM/Core/FileReader.h"
#include "JVM/Core/FileBuffer.h"

std::unique_ptr<FileReader> FileReader::create(std::string_view filename) {
  auto reader = std::make_unique<FileReader>();
  reader->buf = FileBuffer::create(filename);
  return reader;
}
