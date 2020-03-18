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

#include "JVM/VM/Allocator.h"
#include "JVM/VM/Class.h"
#include "JVM/VM/InMemoryObject.h"
#include <cstdlib>
#include <cstring>
#include <map>
#include <mutex>

using MapType = std::map<uint32_t, std::unique_ptr<InMemoryObject>>;

static std::mutex mapLock;
static MapType map;

static uint32_t getLowestKey(const MapType &map) {
  uint32_t lowest = 0;
  for (const auto &[key, _] : map) {
    if (lowest < key)
      break;
    assert(lowest == key);
    lowest++;
  }
  return lowest;
}

static size_t alignUp(size_t align, size_t toAlign) {
  return toAlign + ((align - 1) & -algin);
}

uint32_t jvm::allocate(const jvm::Class &clss) {
  size_t objectSize = clss.objectRepresentation.getObjectSize();
  size_t aligned =
      alignUp(jvm::requiredTypeAlignment, sizeof(InMemoryObject) + objectSize);
  void *const mem = aligned_alloc(jvm::requiredTypeAlignment, aligned);
  assert(mem);
  new (mem) InMemoryObject{clss};
  std::memset(reinterpret_cast<char *>(mem) + sizeof(InMemoryObject), 0,
              objectSize);
  assert(!((reinterpret_cast<uintptr_t>(mem) + sizeof(InMemoryObject)) %
           jvm::requiredTypeAlignment));

  std::scoped_lock X(mapLock);
  uint32_t key = getLowestKey(map);
  map[key] =
      std::unique_ptr<InMemoryObject>(reinterpret_cast<InMemoryObject *>(mem));
  return key;
}

InMemoryObject *jvm::getObject(uint32_t key) {
  std::scoped_lock X(mapLock);
  auto found = map.find(key);
  assert(found != map.end());
  return found->second.get();
}

void jvm::deallocate(uint32_t key) {
  std::scoped_lock X(mapLock);
  auto found = map.find(key);
  assert(found != map.end());
  map.erase(found);
}

size_t jvm::getNumAllocated() {
  std::scoped_lock X(mapLock);
  return map.size();
}

void jvm::deallocateAll() {
  std::scoped_lock X(mapLock);
  map.clear();
}
