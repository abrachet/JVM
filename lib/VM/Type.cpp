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
#include "JVM/Core/ErrorOr.h"
#include "JVM/optional"
#include <algorithm>

static std::optional<Type::BasicType> parseOne(std::string_view &str) {
  auto begin = str.begin();
  Type::BasicType t;
  if (*begin == '[') {
    t.array = true;
    ++begin;
  }
  switch (*begin) {
  case Function:
    [[fallthrough]];
  default:
    return {};
  case Void:
    t.c = Void;
    break;
  case Byte:
    t.c = Byte;
    break;
  case Boolean:
    t.c = Boolean;
  case Short:
    t.c = Short;
    break;
  case Char:
    t.c = Char;
    break;
  case Int:
    t.c = Int;
    break;
  case Long:
    t.c = Long;
    break;
  case Float:
    t.c = Float;
    break;
  case Double:
    t.c = Double;
    break;

  case Object:
    goto object;
  }
  str = begin + 1;
  assert(t.c != Object);
  return t;

object:
  auto semiColon = std::find(++begin, str.end(), ';');
  str = {semiColon + 1, (size_t)std::distance(semiColon + 1, str.end())};
  t.c = Object;
  t.objectName = {begin, semiColon};
  return t;
}

ErrorOr<Type> Type::parseType(std::string_view str) {
  if (str.front() != '(') {
    std::optional<Type::BasicType> typeOrErr = parseOne(str);
    if (typeOrErr)
      return Type(*typeOrErr);
    return std::string("Ill formed type");
  }
  assert(str.front() == '(');
  str = std::string_view(str.begin() + 1, str.size() - 1);
  Type ret;
  ret.function = true;
  while (str.front() != ')') {
    std::optional<Type::BasicType> typeOrErr = parseOne(str);
    if (!typeOrErr)
      return std::string("Ill formed type");
    ret.functionParam.push_back(*typeOrErr);
  }
  assert(str.front() == ')');
  str = std::string_view(str.begin() + 1, str.size() - 1);
  std::optional<Type::BasicType> retTypeOrErr = parseOne(str);
  if (!retTypeOrErr)
    return std::string("Ill formed type");
  ret.type = *retTypeOrErr;
  return ret;
}

size_t Type::BasicType::getStackEntryCount() const {
  if (array)
    return 2;

  switch (c) {
  case Void:
    return 0;
  case Byte:
  case Boolean:
  case Short:
  case Int:
  case Char:
  case Float:
  case Object:
    return 1;
  case Long:
  case Double:
    return 2;
  default:
    assert(0 && "Invalid type");
  }
}
