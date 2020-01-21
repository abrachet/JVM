
#ifndef JVM_VM_JNI_H
#define JVM_VM_JNI_H

#include <cstdint>
#include <vector>

void *getMethodHandle(const char *sym);
void *loadLibrary(const char *path);

uint64_t invoke(void *handle, const std::vector<uint64_t> &arguments);

#endif // JVM_VM_JNI_H
