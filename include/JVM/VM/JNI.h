
#ifndef JVM_VM_JNI_H
#define JVM_VM_JNI_H

void *getMethodHandle(const char *sym);
void *loadLibrary(const char *path);

#endif // JVM_VM_JNI_H
