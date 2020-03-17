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

#include "JVM/VM/ClassLoader.h"
#include "gtest/gtest.h"

TEST(ClassLoader, ObjectRepresentation) {
  if (ClassLoader::classPath.size() < 3) {
    std::string rtJar;
    ASSERT_FALSE(findRTJar(rtJar).size());
    ASSERT_TRUE(rtJar.size());
    ClassLoader::classPath.push_back(rtJar);
  }
  auto classOrError = ClassLoader::loadClass("ObjectRepresentationIJ");
  ASSERT_TRUE(classOrError) << classOrError.getError();
  const auto &objectRep = classOrError->second->objectRepresentation;
  EXPECT_EQ(objectRep.getObjectSize(), 16);
  EXPECT_EQ(objectRep.getFieldOffset(0), 0);
  EXPECT_EQ(objectRep.getFieldOffset(1), 8);
}

TEST(ClassLoader, Name) {
  if (ClassLoader::classPath.size() < 3) {
    std::string rtJar;
    ASSERT_FALSE(findRTJar(rtJar).size());
    ASSERT_TRUE(rtJar.size());
    ClassLoader::classPath.push_back(rtJar);
  }
  auto classOrError = ClassLoader::loadClass("ObjectRepresentationIJ");
  ASSERT_TRUE(classOrError) << classOrError.getError();
  std::string_view name = classOrError->second->name;
  EXPECT_EQ(name, "ObjectRepresentationIJ");
}
