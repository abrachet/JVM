
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

using TypeOrObject = std::pair<Type, std::string>;
using FuncOrSingleType = std::pair<TypeOrObject, std::vector<TypeOrObject>>;

ErrorOr<FuncOrSingleType> parseType(std::string_view);

#endif // JVM_VM_TYPEREADER_H
