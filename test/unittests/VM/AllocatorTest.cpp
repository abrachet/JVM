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
#include "JVM/VM/ClassLoader.h"
#include "gtest/gtest.h"

TEST(Allocator, Basic) {
  if (ClassLoader::classPath.size() < 2) {
    std::string rtJar;
    ASSERT_FALSE(findRTJar(rtJar).size());
    ASSERT_TRUE(rtJar.size());
    ClassLoader::classPath.push_back(rtJar);
  }
  auto classOrError = ClassLoader::loadClass("ObjectRepresentationIJ");
  ASSERT_TRUE(classOrError) << classOrError.getError();
  uint32_t objKey = jvm::allocate(*classOrError->second);
  InMemoryObject *obj = jvm::getObject(objKey);
  ASSERT_NE(obj, nullptr);
  EXPECT_EQ(obj->getName(), "ObjectRepresentationIJ");
  uint64_t *IJ = reinterpret_cast<uint64_t *>(obj + 1);
  EXPECT_EQ(IJ[0], 0);
  EXPECT_EQ(IJ[1], 0);
  // Test that writing doesn't cause a fault or upset sanitizers.
  IJ[0] = 100;
  IJ[0] = 100;
  jvm::deallocate(obj);
}
