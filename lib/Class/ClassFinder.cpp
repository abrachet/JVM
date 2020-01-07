
#include "JVM/Class/ClassFinder.h"
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <zip.h>

static std::pair<zip_t *, std::string> openZip(const std::string &path) {
  using namespace std::string_literals;
  int error = 0;
  zip_t *zip = ::zip_open(path.c_str(), ZIP_RDONLY, &error);
  switch (error) {
  case 0:
    break;
  case ZIP_ER_NOENT:
    return {nullptr, "Path '"s + path + "' does not exist."};
  case ZIP_ER_NOZIP:
  case ZIP_ER_OPEN:
    return {nullptr, "'"s + path + "' could not be opened."};
  default:
    errno = 0;
    return {nullptr, "Unkown error for path: '"s + path + "'."};
  }
  assert(zip);
  return {zip, {}};
}

std::string findRTJar(std::string &path) {
  path = std::getenv("JAVA_HOME");
  if (path.empty())
    return "Can't find rt.jar: JAVA_HOME not set.";
  path += "jre/lib/rt.jar";

  auto [zip, err] = openZip(path);
  if (!zip) {
    path.clear();
    return err;
  }

  assert(err.empty());
  (void)::zip_close(zip);
  return {};
}

std::string registerFromJar(const std::string &path,
                            std::function<void(std::string)> registerFn) {
  auto [zip, err] = openZip(path.c_str());
  if (!zip) {
    assert(!err.empty());
    return err;
  }

  ssize_t numEntries = ::zip_get_num_entries(zip, 0);
  for (ssize_t i = 0; i < numEntries; i++)
    if (const char *str = zip_get_name(zip, i, ZIP_FL_ENC_GUESS); str)
      registerFn(str);

  (void)::zip_close(zip);
  return {};
}

static bool endsWith(const std::string &str, std::string_view search) {
  const char *data = str.c_str();
  assert(!search.data()[search.size()] && "search not null terminated");
  return !strcmp(data + str.size() - search.size(), search.data());
}

static bool jarContainsFile(std::string_view jarPath,
                            std::string_view className) {
  std::string str(jarPath);
  auto [zip, _] = openZip(str);
  if (!zip)
    return false;

  auto *ptr = zip_fopen(zip, className.data(), ZIP_FL_UNCHANGED);
  if (!ptr)
    return false;

  (void)::zip_close(zip);
  return true;
}

#include <iostream>

// TODO: don't rely on file names stat the file to see if its a directory
// and look a the respective magic of a zip file and class file.
ClassLocation findClassLocation(std::string className,
                                const std::vector<std::string> &classPath) {
  assert(!endsWith(className, ".class") && "Invalid className");
  std::string classFilePath = className + ".class";
  for (const std::string &str : classPath) {
#ifdef JVM_CLASSFILE_CLASSPATH_EXTENSION
    if (endsWith(str, ".class") && endsWith(str, className + ".class"))
      return {std::move(className), str};
#endif
    if (endsWith(str, ".jar") && jarContainsFile(str, classFilePath))
      return {std::move(className), str, ClassLocation::InJar};

    std::string path = str;
    if (path.back() != '/')
      path += '/';
    path += classFilePath;
    if (!::access(path.c_str(), F_OK))
      return {std::move(className), std::move(path)};
  }

  return {};
}
