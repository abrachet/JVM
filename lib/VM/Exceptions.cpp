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

#include "JVM/Class/ClassFile.h"
#include "JVM/VM/Allocator.h"
#include "JVM/VM/ClassHierarchyWalker.h"
#include "JVM/VM/ThreadContext.h"

using ExceptionTable = Class::CodeAttribute::ExceptionTable;

static bool withinRange(uint32_t low, uint32_t high, uint32_t query) {
  return query >= low && query <= high;
}

static std::optional<uint32_t>
findCurrentHandler(const ThreadContext &tc,
                   const ExceptionTable &exceptionTable,
                   const ClassHierarchyWalker &walker) {
  using ClassInfo = Class::ConstPool::ClassInfo;
  using Utf8Info = Class::ConstPool::Utf8Info;
  uint32_t relativePc = tc.getRelativePc();
  const auto &clss = tc.getClassFile();
  const auto &constPool = clss.getConstPool();

  for (const auto &entry : exceptionTable) {
    const auto &classInfo = constPool.get<ClassInfo>(entry.catchType);
    std::string_view name = constPool.get<Utf8Info>(classInfo.nameIndex);
    if (withinRange(entry.startPc, entry.endPc, relativePc) &&
        walker.extends(name))
      return entry.handlerPc;
  }
  return {};
}

static ErrorOr<ExceptionTable> findCurrentET(const ThreadContext &tc) {
  const auto &method = tc.getCurrentMethod();
  ErrorOr<Class::CodeAttribute> codeOrErr =
      method.findCodeAttr(tc.getClassFile().getConstPool());
  if (!codeOrErr)
    return codeOrErr.getError();
  assert(codeOrErr->exceptionTableLength == codeOrErr->exceptionTable.size());
  return codeOrErr->exceptionTable;
}

// From VM/Instructions/Control.cpp.
void return_(ThreadContext &);

static std::optional<uint32_t>
unwindFrameOrJumpAddr(ThreadContext &tc, const ClassHierarchyWalker &walker) {
  auto etOrErr = findCurrentET(tc);
  if (etOrErr) {
    std::optional<uint32_t> handler = findCurrentHandler(tc, *etOrErr, walker);
    if (handler)
      return *handler;
  }

  return_(tc);
  return {};
}

void unwindFromException(ThreadContext &tc, uint32_t exceptionRef) {
  InMemoryItem *item = jvm::getAllocatedItem(exceptionRef);
  assert(item && "NullPointerException");
  assert(!item->isArray());
  ClassHierarchyWalker walker(reinterpret_cast<InMemoryObject *>(item)->clss);
  std::optional<uint32_t> handler = 0;

  while (!(handler = unwindFrameOrJumpAddr(tc, walker)))
    ;
  tc.jumpFromStart(*handler);
}
