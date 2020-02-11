
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
  case Short:
  case Int:
  case Char:
  case Float:
    return 1;
  case Long:
  case Double:
  case Object:
    return 2;
  default:
    assert(0 && "Invalid type");
  }
}
