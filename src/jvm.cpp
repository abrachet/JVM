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

// TODO: remove these when String support arrives and no need to ignore
// invokespecial
#include "JVM/Core/BigEndianByteReader.h"
#include "JVM/VM/Instructions.h"

extern "C" void Java___JVM_internal_Start_exit(void *, int exitCode) {
  std::exit(exitCode);
}

#ifdef TESTING
extern "C" void Java_Natives_exit(void *, int exitCode) { std::exit(exitCode); }
#endif

template <typename T> static void dieIfError(const ErrorOr<T> &error) {
  if (!error) {
    std::fprintf(stderr, "JVM Fatal error: %s", error.getError().c_str());
    std::exit(1);
  }
}

class MethodLoader {
  const LoadedClass &loadedClass;
  const Class::Method &method;

  MethodLoader(const LoadedClass &loadedClass, const Class::Method &method)
      : loadedClass(loadedClass), method(method) {}

public:
  static ErrorOr<MethodLoader> create(std::string_view className,
                                      std::string_view name,
                                      std::string_view type) {
    ErrorOr<LoadedClass &> loadedClass = ClassLoader::loadClass(className);
    if (!loadedClass)
      return loadedClass.getError();

    const auto &classFile = *loadedClass->second->loadedClass;
    const auto methodOrErr = classFile.findMethodByNameType(name, type);
    if (!methodOrErr)
      return methodOrErr.getError();
    return MethodLoader(*loadedClass, *methodOrErr);
  }

  ErrorOr<const void *> findCode() const {
    const auto &cp = loadedClass.second->loadedClass->getConstPool();
    ErrorOr<Class::CodeAttribute> codeOrErr = method.findCodeAttr(cp);
    if (!codeOrErr)
      return codeOrErr.getError();
    return codeOrErr->code;
  }

  ErrorOr<Frame> createFrameFromMethod(const void *returnAddr,
                                       void *frameStart) const {
    ErrorOr<const void *> codeOrErr = findCode();
    if (!codeOrErr)
      return codeOrErr.getError();
    return Frame(loadedClass.second->name, returnAddr, frameStart, *codeOrErr,
                 method.nameIndex, method.descriptorIndex);
  }
};

constexpr std::string_view startClassName = "__JVM_internal_Start";

[[noreturn]] static void
startJVM(std::string_view mainClass, uint32_t arg,
         std::string_view mainType = "([Ljava/lang/String;)V") {
  ErrorOr<Stack> mainStack = Stack::createStack();
  dieIfError(mainStack);
  ThreadContext mainThread(std::move(*mainStack));

  ErrorOr<MethodLoader> startLoader =
      MethodLoader::create(startClassName, "start", "()V");
  dieIfError(startLoader);
  ErrorOr<Frame> startFrame =
      startLoader->createFrameFromMethod(nullptr, mainThread.stack.sp);
  dieIfError(startFrame);
  mainThread.pushFrame(std::move(*startFrame));

  ErrorOr<MethodLoader> mainLoader =
      MethodLoader::create(mainClass, "main", mainType);
  dieIfError(mainLoader);
  const void *startReturn = mainThread.currentFrame().pcStart;
  ErrorOr<Frame> mainFrame =
      mainLoader->createFrameFromMethod(startReturn, mainThread.stack.sp);
  dieIfError(mainFrame);
  mainThread.pushFrame(std::move(*mainFrame));

  mainThread.storeInLocal<1>(0, arg);
  mainThread.pc = *mainLoader->findCode();
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
  // TOOD: use /tmp/JVM_FileCache, but that's broken right now.
  FileCache fileCache("./JVM_FileCache");
  ClassLoader::registerFileCache(fileCache);

  std::string_view argv1(argv[1]);
  // TODO: better command line args handling
  if (argv1 == "-Xintmain") {
    if (argc < 4) {
      std::fputs("no class file or arg specified for -Xintmain\n", stderr);
      return 1;
    }
    startJVM(argv[2], std::atoi(argv[3]), "(I)V");
  } else if (argv1 == "-Xnoinvokespecial") {
    if (argc < 3) {
      std::fputs("no class file specified for -Xnoinvokespecial\n", stderr);
      return 1;
    }
    instructions[Instructions::invokespecial] =
        +[](ThreadContext &tc) { readFromPointer<uint16_t>(tc.pc); };
    startJVM(argv[2], 0);
  } else {
    // TODO pass command line args
    startJVM(argv[1], 0);
  }
  __builtin_unreachable();
}
