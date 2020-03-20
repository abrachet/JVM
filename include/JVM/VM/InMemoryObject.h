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

#ifndef JVM_VM_INMEMORYOBJECT_H
#define JVM_VM_INMEMORYOBJECT_H

#include "JVM/VM/Class.h"
#include "JVM/VM/ClassLoader.h"
#include "JVM/VM/Stack.h"
#include "JVM/string_view"
#include <mutex>

namespace jvm {
constexpr size_t requiredTypeAlignment = Stack::stackEntryBytes * 2;
}

struct alignas(jvm::requiredTypeAlignment) InMemoryItem {
  std::recursive_mutex monitor;

  virtual size_t getThisSize() const = 0;
  // We don't compile with rtti.
  virtual bool isArray() const { return false; }

  virtual ~InMemoryItem() {}

  void *getThisptr() const {
    const char *addr = reinterpret_cast<const char *>(this);
    addr += getThisSize();
    return const_cast<void *>(reinterpret_cast<const void *>(addr));
  }

protected:
  InMemoryItem() {}
};

struct alignas(jvm::requiredTypeAlignment) InMemoryObject
    : public InMemoryItem {
  const jvm::Class &clss;

  InMemoryObject(const jvm::Class &clss) : clss(clss) {}

  size_t getThisSize() const override { return sizeof(InMemoryObject); }

  std::string_view getName() const { return clss.name; }

  ObjectRepresentation getObjectRepresentation() {
    return clss.objectRepresentation;
  }
};

struct alignas(jvm::requiredTypeAlignment) InMemoryArray : public InMemoryItem {
  Type type;
  size_t length;

  size_t getThisSize() const override { return sizeof(InMemoryArray); }
  bool isArray() const override { return true; }

  InMemoryArray(Type type, size_t length) : type(type), length(length) {}
};

#endif // JVM_VM_INMEMORYOBJECT_H