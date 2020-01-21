
#ifndef JVM_VM_TYPEREADER_H
#define JVM_VM_TYPEREADER_H

#include "JVM/Core/ErrorOr.h"
#include "JVM/string_view"
#include <string>
#include <utility>
#include <vector>

struct Type {
  char c;

  constexpr Type() : c(0) {}
  constexpr Type(char c) : c(c) {}
  size_t getStackEntryCount() const;
  constexpr operator char() const { return c; }
};

constexpr Type Function('(');
constexpr Type Void('V');
constexpr Type Byte('B');
constexpr Type Short('S');
constexpr Type Int('I');
constexpr Type Long('J');
constexpr Type Char('C');
constexpr Type Float('F');
constexpr Type Double('D');
constexpr Type Object('L');

#if 0
struct Types {
  enum Type : char {
    Function = '(',
    Void = 'V',
    Byte = 'B',
    Short = 'S',
    Int = 'I',
    Long = 'J',
    Char = 'C',
    Float = 'F',
    Double = 'D',
    Object = 'L'
  };

  Type type;

  Types() : type(static_cast<Type>(0)) {}
  Types(Type type) : type(type) {}
  explicit Types(uint64_t c) : type(static_cast<Type>(c)) {}

  bool operator==(Type otherType) const {
    return otherType == type;
  }

  bool operator==(Types other) const {
    return other.type == type;
  }

  operator Type() const {
    return type;
  }
};
#endif

using TypeOrObject = std::pair<Type, std::string>;
using FuncOrSingleType = std::pair<TypeOrObject, std::vector<TypeOrObject>>;

ErrorOr<FuncOrSingleType> parseType(std::string_view);

#endif // JVM_VM_TYPEREADER_H
