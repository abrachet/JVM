
#ifndef JVM_CLASS_CLASSFINDER_H
#define JVM_CLASS_CLASSFINDER_H

#include "JVM/Core/FileBuffer.h"
#include "JVM/string_view"
#include <cassert>
#include <functional>
#include <string>
#include <vector>

struct ClassLocation {
  enum LocationType { NoExist, File, InJar };

  std::string className;
  std::string path;
  LocationType type = NoExist;

  ClassLocation() = default;
  ClassLocation(std::string className, std::string path,
                LocationType type = File)
      : className(className), path(path), type(type) {}
};

class ZipFileBuffer : public FileBuffer {
  size_t fileSize;
  char *mappedFile;

  ZipFileBuffer() {}

public:
  static std::unique_ptr<FileBuffer> create(std::string_view zipFile,
                                            std::string_view entry);
  ~ZipFileBuffer() override;

  operator const char *() const override { return mappedFile; }
  operator char *() override { return mappedFile; }
  size_t size() const override { return fileSize; }
};

std::string findRTJar(std::string &path);

std::string registerFromJar(const std::string &path,
                            std::function<void(std::string)> registerFn);

ClassLocation findClassLocation(std::string className,
                                const std::vector<std::string> &classPath);

#endif // JVM_CLASS_CLASSFINDER_H
