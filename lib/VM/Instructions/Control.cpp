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

#include "Control.h"

template <size_t TypeWidth> void treturn(ThreadContext &tc) {
  if (tc.currentFrame().syncronizedMethod)
    tc.getLoadedClass().second->monitor.unlock();
  uint64_t arg;
  if constexpr (TypeWidth)
    arg = tc.stack.pop<TypeWidth>();
  Frame frame = tc.popFrame();
  tc.pc = frame.returnAddress;
  if constexpr (TypeWidth)
    tc.stack.push<TypeWidth>(arg);
}

void ireturn(ThreadContext &tc) { return treturn<1>(tc); }
void return_(ThreadContext &tc) { return treturn<0>(tc); }
