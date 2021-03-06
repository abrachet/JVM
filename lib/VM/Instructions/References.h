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

#ifndef INS_REFERENCES_H
#define INS_REFERENCES_H

#include "Instruction.h"
#include "JVM/VM/Instructions.h"

void getstatic(ThreadContext &);
void putstatic(ThreadContext &);
void getfield(ThreadContext &);
void putfield(ThreadContext &);
void invokevirtual(ThreadContext &);
void invokespecial(ThreadContext &);
void invokestatic(ThreadContext &);
void new_(ThreadContext &);
void newarray(ThreadContext &);
void arraylength(ThreadContext &);
void athrow(ThreadContext &);
void instanceof (ThreadContext &);

#endif // INS_REFERENCES_H
