
#include "JVM/VM/ClassLoader.h"
#include "JVM/Class/ClassFileReader.h"
#include "JVM/Class/ClassFinder.h"
#include "JVM/Core/Defer.h"
#include "JVM/VM/ObjectRepresentation.h"
#include <string>

using namespace std::string_literals;

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

std::string ClassLoader::loadSuperClasses(ClassLoader::Class &clazz) {
  auto &constPool = clazz.loadedClass->getConstPool();
  auto loadClassFromPool = [&](uint64_t index) -> std::string {
    if (index == 0)
      return {};
    if (constPool.getEntries()[index]->tag != ::Class::ConstPool::Class)
      return "Invalid class file. Super or interface field does not refer to a "
             "ClassInfo structure.";
    const auto &superClass =
        constPool.get<::Class::ConstPool::ClassInfo>(index);
    const auto &name =
        constPool.get<::Class::ConstPool::Utf8Info>(superClass.nameIndex);
    auto classOrError = loadClass({reinterpret_cast<const char *>(name.bytes),
                                   static_cast<size_t>(name.length)});
    if (classOrError)
      clazz.superClasses.emplace_back(classOrError.get());
    return classOrError.getError();
  };

  uint16_t super = clazz.loadedClass->getSuperClass();
  if (std::string str = loadClassFromPool(super); str.size())
    return str;
  for (uint16_t interface : clazz.loadedClass->getInterfaces())
    if (std::string str = loadClassFromPool(interface); str.size())
      return str;
  return {};
}

ErrorOr<ClassLoader::LoadedClass &>
ClassLoader::loadClass(const std::string_view fullClassName) {
  std::string className(fullClassName.data(), fullClassName.size());
  loadedClassesMutex.lock_shared();
  if (auto it = loadedClasses.find(className); it != loadedClasses.end()) {
    loadedClassesMutex.unlock_shared();
    auto &[lock, lClass] = it->second;
    auto &[cv, mtx] = lock;
    std::unique_lock l(mtx);
    cv.wait(l, [&it] { return it->second.second.state == Class::Loaded; });
    return it->second;
  }
  loadedClassesMutex.unlock_shared();
  ClassLocation loc = findClassLocation(className, classPath);
  if (loc.type == ClassLocation::NoExist)
    return "Class '"s + className + "' does not exist.";

  loadedClassesMutex.lock();
  auto &loadedClass = loadedClasses[className];
  loadedClassesMutex.unlock();

  auto &[lock, lClass] = loadedClass;
  auto &[cv, mtx] = lock;
  std::unique_lock l(mtx);

  auto [classFile, err] = readClass(loc);
  if (err.size()) {
    lClass.state = Class::Erroneous;
    goto end;
  }

  lClass.name = fullClassName;
  lClass.loadedClass = std::move(classFile);
  lClass.location = loc;
  {
    auto orOrErr =
        ObjectRepresentation::createFromClassFile(*lClass.loadedClass);
    if (!orOrErr)
      return orOrErr.getError();
    lClass.objectRepresentation = *orOrErr;
  }

  if (std::string str = loadSuperClasses(lClass); str.size())
    return str;

  lClass.state = Class::Loaded;
end:
  auto _ = defer([&cv = cv] { cv.notify_all(); });
  return loadedClass;
}

ErrorOr<ClassLoader::LoadedClass &>
ClassLoader::getLoadedClass(const std::string_view fullClassName) {
  loadedClassesMutex.lock();
  auto it = loadedClasses.find(
      std::string(fullClassName.data(), fullClassName.size()));
  auto end = loadedClasses.end();
  loadedClassesMutex.unlock();
  if (it != end)
    return it->second;
  return std::string("Class not found");
}
