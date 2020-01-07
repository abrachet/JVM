
#include "JVM/Class/ClassLoader.h"
#include "JVM/Class/ClassFinder.h"
#include <string>

using namespace std::string_literals;

std::vector<std::string> ClassLoader::classPath = {"."};

ClassLoader::LoadedClassOrErr
ClassLoader::loadClass(const std::string_view fullClassName) {
  ClassLocation loc = findClassLocation(fullClassName.data(), classPath);
  if (loc.type == ClassLocation::NoExist)
    return {nullptr, "Class '"s + fullClassName.data() + "' does not exist."};

  return {};
}
