
#include "JVM/Class/ClassFinder.h"
#include <cassert>
#include <cstdlib>
#include <unistd.h>
#include <zip.h>

static std::pair<zip_t *, std::string> openZip(const std::string &path) {
  using namespace std::string_literals;
  int error;
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

std::string findClassLocation(std::string_view className,
                              const std::vector<std::string_view> &classPath) {
  return {};
}
