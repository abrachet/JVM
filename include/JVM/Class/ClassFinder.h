
#ifndef JVM_CLASS_CLASSFINDER_H
#define JVM_CLASS_CLASSFINDER_H

#include "JVM/string_view"
#include <functional>
#include <string>
#include <vector>

std::string findRTJar(std::string &path);

std::string registerFromJar(const std::string &path,
                            std::function<void(std::string)> registerFn);

std::string findClassLocation(std::string_view className,
                              const std::vector<std::string_view> &classPath);

#endif // JVM_CLASS_CLASSFINDER_H
