
#include "JVM/VM/JNI.h"
#include <dlfcn.h>
#include <string>

void *getMethodHandle(const char *sym) { return dlsym(RTLD_DEFAULT, sym); }

void *loadLibrary(const char *name) {
  return dlopen(name, RTLD_LAZY | RTLD_GLOBAL);
}
