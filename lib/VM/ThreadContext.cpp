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

#include "JVM/VM/ThreadContext.h"
#include "JVM/Core/BigEndianByteReader.h"
#include "JVM/VM/Instructions.h"
#include <iostream>

void ThreadContext::callNext() {
  assert(pc && "pc is nullptr");
  uint8_t ins = readFromPointer<uint8_t>(pc);
  // Instructions::Instruction instruc(ins);
  std::cerr << (int)ins << '\n';
  return instructions[ins](*this);
}

LoadedClass &ThreadContext::getLoadedClass() {
  ErrorOr<LoadedClass &> loadedClassOrErr =
      ClassLoader::getLoadedClass(getCurrentClassName());
  assert(loadedClassOrErr && "class was not loaded");
  return *loadedClassOrErr;
}
