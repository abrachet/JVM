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

#ifndef JVM_VM_TYPEREADER_H
#define JVM_VM_TYPEREADER_H

#include "JVM/Core/ErrorOr.h"
#include "JVM/string_view"
#include <string>
#include <utility>
#include <vector>

struct Type {
  struct BasicType {
    char c;
    bool array = false;
    std::string objectName;

    BasicType() : c(0) {}
    BasicType(char c) : c(c) {}
    BasicType(char c, std::string str) : c(c), objectName(str) {}
    size_t getStackEntryCount() const;
    bool operator==(BasicType other) const {
      return c == other.c && array == other.array;
    }
  };

private:
  bool function = false;
  // This is the return type if it is a function.
  BasicType type;
  std::vector<BasicType> functionParam;

  Type() = default;
  Type(BasicType type) : type(type) {}

public:
  explicit Type(char c) : type(c) {}
  static ErrorOr<Type> parseType(std::string_view typeName);

  bool isFunctionType() const { return function; }
  bool isArrayType() const {
    assert(!isFunctionType());
    return type.array;
  }
  bool isBasicType() const { return !isFunctionType() && !isArrayType(); }

  BasicType getReturnType() const {
    assert(isFunctionType());
    return type;
  }

  const std::vector<BasicType> &getFunctionArgs() const {
    assert(isFunctionType());
    return functionParam;
  }

  operator BasicType() const {
    assert(!isFunctionType());
    return type;
  }

  operator char() const {
    assert(isBasicType());
    return type.c;
  }

  size_t getStackEntryCount() const {
    assert(isBasicType());
    return type.getStackEntryCount();
  }
};

constexpr char Function('(');
constexpr char Void('V');
constexpr char Byte('B');
constexpr char Short('S');
constexpr char Int('I');
constexpr char Long('J');
constexpr char Char('C');
constexpr char Float('F');
constexpr char Double('D');
constexpr char Object('L');

#endif // JVM_VM_TYPEREADER_H
