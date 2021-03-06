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

#ifndef JVM_VM_STACK_H
#define JVM_VM_STACK_H

#include "JVM/Core/ErrorOr.h"
#include "JVM/Core/type_traits.h"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <system_error>
#include <unistd.h>

struct Stack {
  // TODO: The error callback should be handled by mprotecting a guard page.
  using CallbackType = std::function<void(Stack &)>;
  CallbackType stackOverflowCallback = nullptr;
  void *stack = nullptr;
  size_t size;
  void *sp;

  static ErrorOr<Stack> createStack(size_t size = 0x200000,
                                    CallbackType cb = nullptr);

  template <int entries> void push(uint64_t toPush);
  template <> void push<1>(uint64_t toPush) { push<uint32_t>(toPush); }
  template <> void push<2>(uint64_t toPush) { push<uint64_t>(toPush); }

  template <int entries> uint64_t pop();
  template <> uint64_t pop<1>() { return pop<uint32_t>(); }
  template <> uint64_t pop<2>() { return pop<uint64_t>(); }

  Stack(const Stack &) = delete;
  Stack(Stack &&other)
      : stackOverflowCallback(other.stackOverflowCallback), stack(other.stack),
        size(other.size), sp(other.sp) {
    other.stack = nullptr;
  }

  ~Stack();

private:
  Stack() = default;

  // TODO (During addition of JIT): Hand code this to keep all variables in
  // registers so that it never messes with the programs stack.
  template <typename T> void push(T toPush) {
    if (reinterpret_cast<T *>(sp) - 1 < reinterpret_cast<T *>(stack))
      stackOverflowCallback(*this);
    T *top = reinterpret_cast<T *>(sp) - 1;
    *top = static_cast<T>(toPush);
    sp = reinterpret_cast<T *>(sp) - 1;
  }

  template <typename T> uint64_t pop() {
    // TODO: assert for stack underflow.
    T *top = reinterpret_cast<T *>(sp);
    uint64_t ret = *top;
    sp = top + 1;
    return ret;
  }

  template <size_t> struct Entry;
  template <> struct Entry<1> : std::type_identity<uint32_t> {};
  template <> struct Entry<2> : std::type_identity<uint64_t> {};

public:
  template <size_t Size> using EntryType = typename Entry<Size>::type;

  constexpr static size_t stackEntryBytes = sizeof(void *) / 2;

  // TODO: Probably impossible by now to remove this engrained assumption.
  static_assert(stackEntryBytes == 4, "64 bit machine expected");

  static constexpr bool validEntrySize(int size) {
    return size == 1 || size == 2;
  }
};

#endif // JVM_VM_STACK_H
