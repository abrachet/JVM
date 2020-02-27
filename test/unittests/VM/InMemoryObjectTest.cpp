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

#include "JVM/VM/InMemoryObject.h"
#include "gtest/gtest.h"

TEST(InMemorObject, GetThis) {
  jvm::Class *ptr = nullptr;
  InMemoryObject obj{*ptr};
  // Be careful of pointer provence issues, uintptr comparassions are the safest
  // bet to not be optimized out by the compiler.
  uintptr_t addr = reinterpret_cast<uintptr_t>(std::addressof(obj));
  addr += sizeof(obj);
  EXPECT_EQ(addr, reinterpret_cast<uintptr_t>(obj.getThisptr()));
}
