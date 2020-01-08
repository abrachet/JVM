
#include "JVM/Class/ClassLoader.h"
#include "JVM/Class/ClassFileReader.h"
#include "JVM/Class/ClassFinder.h"
#include <string>

using namespace std::string_literals;

std::vector<std::string> ClassLoader::classPath = {"."};
std::unordered_map<std::string, ClassLoader::LoadedClass>
    ClassLoader::loadedClasses;

static ClassFileReader::ClassFileOrError readClass(const ClassLocation &loc) {
  assert(loc.type != ClassLocation::NoExist);
  if (loc.type == ClassLocation::File) {
    ClassFileReader reader(loc.path);
    return reader.read();
  }

  assert(loc.type == ClassLocation::InJar);
  std::unique_ptr<FileBuffer> buf =
      ZipFileBuffer::create(loc.path, loc.className + ".class");
  if (!buf)
    return {nullptr, "Couldn't extract '"s + loc.className + "' from jar."};
  ClassFileReader reader(std::move(buf));
  return reader.read();
}

ClassLoader::LoadedClassOrErr
ClassLoader::loadClass(const std::string_view fullClassName) {
  static ClassLoader::LoadedClass nullLoadedClass;

  std::string className = fullClassName.data();
  auto it = loadedClasses.find(className);
  if (it != loadedClasses.end())
    return {it->second, {}};

  ClassLocation loc = findClassLocation(className, classPath);
  if (loc.type == ClassLocation::NoExist)
    return {nullLoadedClass, "Class '"s + className + "' does not exist."};

  auto &loadedClass = loadedClasses[className];
  auto &[lock, lClass] = loadedClass;

  // TODO: Need to aquire lock here.

  auto [classFile, err] = readClass(loc);
  if (err.size()) {
    lClass.state = Class::Erroneous;
    goto end;
  }

  lClass.loadedClass = std::move(classFile);
  lClass.location = loc;

  lClass.state = Class::Loaded;
end:
  return {loadedClass, {}};
}
