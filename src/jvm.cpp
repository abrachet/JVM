// Copyright 2020 Alex Brachet
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "JVM/Core/ErrorOr.h"
#include "JVM/VM/ClassLoader.h"
#include "JVM/VM/Stack.h"
#include "JVM/VM/ThreadContext.h"
#include "JVM/string_view"
#include <cstdio>
#include <cstdlib>

extern "C" void Java___JVM_internal_Start_exit(void *, int exitCode) {
  std::exit(exitCode);
}

#ifdef TESTING
extern "C" void Java_Natives_exit(void *, int exitCode) {
  std::exit(exitCode);
}
#endif

template <typename T> static void dieIfError(const ErrorOr<T> &error) {
  if (!error) {
    std::fprintf(stderr, "JVM Fatal error: %s", error.getError().c_str());
    std::exit(1);
  }
}

static ErrorOr<Class::CodeAttribute> loadMethod(std::string_view className,
                                                std::string_view name,
                                                std::string_view type) {
  ErrorOr<LoadedClass &> loadedClass = ClassLoader::loadClass(className);
  if (!loadedClass)
    return loadedClass.getError();

  const auto &classFile = *loadedClass->second->loadedClass;
  const auto mainMethodOrErr = classFile.findMethodByNameType(name, type);
  if (!mainMethodOrErr)
    return mainMethodOrErr.getError();
  ErrorOr<Class::CodeAttribute> codeOrErr =
      mainMethodOrErr->findCodeAttr(classFile.getConstPool());
  if (!codeOrErr)
    return codeOrErr.getError();
  return *codeOrErr;
}

static ErrorOr<const void *> loadMain(std::string_view className) {
  ErrorOr<Class::CodeAttribute> mainCodeOrErr =
      loadMethod(className, "main", "([Ljava/lang/String;)V");
  if (!mainCodeOrErr)
    return mainCodeOrErr.getError();
  return reinterpret_cast<const void *>(mainCodeOrErr->code);
}

constexpr std::string_view startClassName = "__JVM_internal_Start";

static ErrorOr<const void *> loadStart() {
  ErrorOr<Class::CodeAttribute> startCodeOrErr =
      loadMethod(startClassName, "start", "()V");
  if (!startCodeOrErr)
    return startCodeOrErr.getError();
  return reinterpret_cast<const void *>(startCodeOrErr->code);
}

[[noreturn]] static void startJVM(std::string_view mainClass) {
  ErrorOr<Stack> mainStack = Stack::createStack();
  dieIfError(mainStack);
  ThreadContext mainThread(std::move(*mainStack));

  ErrorOr<const void *> startAddr = loadStart();
  dieIfError(startAddr);
  mainThread.pushFrame(Frame(startClassName, nullptr, mainThread.stack.sp));

  ErrorOr<const void *> mainAddr = loadMain(mainClass);
  dieIfError(mainAddr);
  mainThread.pushFrame(Frame(mainClass, *startAddr, mainThread.stack.sp));
  mainThread.pc = *mainAddr;
  for (;;)
    mainThread.callNext();
}

int main(int argc, char **argv) {
  if (argc < 2) {
    std::fputs("no class file specifed", stderr);
    return 1;
  }
  std::string rtJar;
  findRTJar(rtJar);
  ClassLoader::classPath.push_back(rtJar);
  startJVM(argv[1]);
  __builtin_unreachable();
}
