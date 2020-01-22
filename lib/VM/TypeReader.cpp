
#include "JVM/VM/TypeReader.h"
#include "JVM/Core/ErrorOr.h"
#include "JVM/optional"
#include <algorithm>

static std::optional<TypeOrObject> parseOne(std::string_view &str) {
  auto begin = str.begin();
  Type t;
  switch (Type(*begin)) {
  case Function:
    [[fallthrough]];
  default:
    return {};
  case Void:
    t = Void;
    break;
  case Byte:
    t = Byte;
    break;
  case Short:
    t = Short;
    break;
  case Char:
    t = Char;
    break;
  case Int:
    t = Int;
    break;
  case Long:
    t = Long;
    break;
  case Float:
    t = Float;
    break;
  case Double:
    t = Double;
    break;

  case Object:
    goto object;
  }
  str = str.begin() + 1;
  return TypeOrObject(t, {});

object:
  auto semiColon = std::find(++begin, str.end(), ';');
  str = {semiColon + 1, (size_t)std::distance(semiColon + 1, str.end())};
  return TypeOrObject(Object, {begin, semiColon});
}

ErrorOr<FuncOrSingleType> parseType(std::string_view str) {
  if (str.front() != '(') {
    std::optional<TypeOrObject> typeOrErr = parseOne(str);
    if (typeOrErr)
      return FuncOrSingleType{*typeOrErr, {}};
    return std::string("Ill formed type");
  }
  assert(str.front() == '(');
  str = std::string_view(str.begin() + 1, str.size() - 1);
  std::vector<TypeOrObject> types(1);
  while (str.front() != ')') {
    std::optional<TypeOrObject> typeOrErr = parseOne(str);
    if (!typeOrErr)
      return std::string("Ill formed type");
    types.push_back(*typeOrErr);
  }
  assert(str.front() == ')');
  str = std::string_view(str.begin() + 1, str.size() - 1);
  std::optional<TypeOrObject> typeOrErr = parseOne(str);
  if (!typeOrErr)
    return std::string("Ill formed type");
  types[0] = *typeOrErr;
  return FuncOrSingleType(TypeOrObject(Function, {}), types);
}

size_t Type::getStackEntryCount() const {
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
