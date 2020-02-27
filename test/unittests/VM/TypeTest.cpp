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

#include "JVM/VM/Type.h"
#include "gtest/gtest.h"

TEST(TypeReader, Basic) {
  {
    ErrorOr<Type> type = Type::parseType("I");
    ASSERT_TRUE(type);
    EXPECT_FALSE(type->isFunctionType());
    EXPECT_TRUE(type->isBasicType());
    EXPECT_EQ(*type, Int);
  }
  {
    ErrorOr<Type> type = Type::parseType("J");
    ASSERT_TRUE(type);
    EXPECT_FALSE(type->isFunctionType());
    EXPECT_TRUE(type->isBasicType());
    EXPECT_EQ(*type, Long);
  }
  {
    ErrorOr<Type> type = Type::parseType("B");
    ASSERT_TRUE(type);
    EXPECT_FALSE(type->isFunctionType());
    EXPECT_TRUE(type->isBasicType());
    EXPECT_EQ(*type, Byte);
  }
  {
    ErrorOr<Type> type = Type::parseType("S");
    ASSERT_TRUE(type);
    EXPECT_FALSE(type->isFunctionType());
    EXPECT_TRUE(type->isBasicType());
    EXPECT_EQ(*type, Short);
  }
  {
    ErrorOr<Type> type = Type::parseType("C");
    ASSERT_TRUE(type);
    EXPECT_FALSE(type->isFunctionType());
    EXPECT_TRUE(type->isBasicType());
    EXPECT_EQ(*type, Char);
  }
  {
    ErrorOr<Type> type = Type::parseType("F");
    ASSERT_TRUE(type);
    EXPECT_FALSE(type->isFunctionType());
    EXPECT_TRUE(type->isBasicType());
    EXPECT_EQ(*type, Float);
  }
  {
    ErrorOr<Type> type = Type::parseType("D");
    ASSERT_TRUE(type);
    EXPECT_FALSE(type->isFunctionType());
    EXPECT_TRUE(type->isBasicType());
    EXPECT_EQ(*type, Double);
  }
}

TEST(TypeReader, Object) {
  ErrorOr<Type> typeOrErr = Type::parseType("Ljava/lang/Object;");
  ASSERT_TRUE(typeOrErr);
  Type::BasicType type = *typeOrErr;
  EXPECT_EQ(type, Object);
  EXPECT_EQ(type.objectName, "java/lang/Object");
}

TEST(TypeReader, Array) {
  {
    ErrorOr<Type> typeOrErr = Type::parseType("[I");
    ASSERT_TRUE(typeOrErr);
    EXPECT_FALSE(typeOrErr->isFunctionType());
    EXPECT_FALSE(typeOrErr->isBasicType());
    Type::BasicType type = *typeOrErr;
    EXPECT_TRUE(type.array);
    EXPECT_EQ(type.c, Int);
  }
  {
    ErrorOr<Type> typeOrErr = Type::parseType("[Ljava/lang/Object;");
    ASSERT_TRUE(typeOrErr);
    EXPECT_FALSE(typeOrErr->isFunctionType());
    EXPECT_FALSE(typeOrErr->isBasicType());
    Type::BasicType type = *typeOrErr;
    EXPECT_TRUE(type.array);
    EXPECT_EQ(type.c, Object);
    EXPECT_EQ(type.objectName, "java/lang/Object");
  }
}

TEST(TypeReader, BasicFunction) {
  ErrorOr<Type> typeOrErr = Type::parseType("(I)J");
  ASSERT_TRUE(typeOrErr);
  ASSERT_TRUE(typeOrErr->isFunctionType());
  EXPECT_EQ(typeOrErr->getReturnType(), Long);
  const auto &params = typeOrErr->getFunctionArgs();
  ASSERT_EQ(params.size(), 1);
  EXPECT_EQ(params[0], Int);
}

TEST(TypeReader, ComplexFunction) {
  {
    ErrorOr<Type> typeOrErr =
        Type::parseType("(Ljava/lang/Object;Ljava/lang/String;C)J");
    ASSERT_TRUE(typeOrErr);
    const Type &type = *typeOrErr;
    ASSERT_TRUE(type.isFunctionType());
    EXPECT_EQ(type.getReturnType(), Long);
    const auto &params = type.getFunctionArgs();
    ASSERT_EQ(params.size(), 3);
    EXPECT_EQ(params[0], Object);
    EXPECT_EQ(params[0].objectName, "java/lang/Object");
    EXPECT_EQ(params[1], Object);
    EXPECT_EQ(params[1].objectName, "java/lang/String");
    EXPECT_EQ(params[2], Char);
    EXPECT_EQ(params[2].objectName, "");
  }
  {
    ErrorOr<Type> typeOrErr = Type::parseType("([Ljava/lang/Object;[I)[C");
    ASSERT_TRUE(typeOrErr);
    EXPECT_TRUE(typeOrErr->isFunctionType());
    auto retType = typeOrErr->getReturnType();
    EXPECT_TRUE(retType.array);
    EXPECT_EQ(retType.c, Char);
    const auto &params = typeOrErr->getFunctionArgs();
    ASSERT_EQ(params.size(), 2);
    auto type = params[0];
    EXPECT_TRUE(type.array);
    EXPECT_EQ(type.c, Object);
    EXPECT_EQ(type.objectName, "java/lang/Object");
    type = params[1];
    EXPECT_TRUE(type.array);
    EXPECT_EQ(type.c, Int);
  }
}

TEST(TypeReader, TypeSize) {
  EXPECT_EQ(Type(Void).getStackEntryCount(), 0);
  EXPECT_EQ(Type(Byte).getStackEntryCount(), 1);
  EXPECT_EQ(Type(Short).getStackEntryCount(), 1);
  EXPECT_EQ(Type(Int).getStackEntryCount(), 1);
  EXPECT_EQ(Type(Long).getStackEntryCount(), 2);
  EXPECT_EQ(Type(Char).getStackEntryCount(), 1);
  EXPECT_EQ(Type(Float).getStackEntryCount(), 1);
  EXPECT_EQ(Type(Double).getStackEntryCount(), 2);
  EXPECT_EQ(Type(Object).getStackEntryCount(), 2);
}
