
#ifndef JVM_CLASS_CLASSFINDER_H
#define JVM_CLASS_CLASSFINDER_H

#include "JVM/string_view"
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

std::string findRTJar(std::string &path);

std::string registerFromJar(const std::string &path,
                            std::function<void(std::string)> registerFn);

ClassLocation findClassLocation(std::string className,
                                const std::vector<std::string> &classPath);

#endif // JVM_CLASS_CLASSFINDER_H
