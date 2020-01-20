
#ifndef JVM_VM_TYPEREADER_H
#define JVM_VM_TYPEREADER_H

#include "JVM/Core/ErrorOr.h"
#include "JVM/string_view"
#include <string>
#include <utility>
#include <vector>

enum Types : char {
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

using Type = std::pair<Types, std::string>;
using FuncOrSingleType = std::pair<Type, std::vector<Type>>;

ErrorOr<FuncOrSingleType> parseType(std::string_view);

#endif // JVM_VM_TYPEREADER_H
