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

#ifndef JVM_CORE_DEFER_H
#define JVM_CORE_DEFER_H

#include <type_traits>
#include <utility>

template <typename Callable> struct ScopedExit {
  Callable c;

  ScopedExit(Callable &&c) : c(c) {}
  ~ScopedExit() { c(); }
};

template <typename Callable> [[nodiscard]] auto defer(Callable &&callable) {
  return ScopedExit<typename std::decay<Callable>::type>(
      std::forward<Callable>(callable));
}

#endif // JVM_CORE_DEFER_H
