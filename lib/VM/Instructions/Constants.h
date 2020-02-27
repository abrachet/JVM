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

#ifndef INS_CONSTANTS_H
#define INS_CONSTANTS_H

#include "Instruction.h"
#include "JVM/VM/Instructions.h"

void nop(ThreadContext &);
void aconst_null(ThreadContext &);
void iconst_m1(ThreadContext &);
void iconst_0(ThreadContext &);
void iconst_1(ThreadContext &);
void iconst_2(ThreadContext &);
void iconst_3(ThreadContext &);
void iconst_4(ThreadContext &);
void iconst_5(ThreadContext &);
void lconst_0(ThreadContext &);
void lconst_1(ThreadContext &);
void fconst_0(ThreadContext &);
void fconst_1(ThreadContext &);
void fconst_2(ThreadContext &);
void dconst_0(ThreadContext &);
void dconst_1(ThreadContext &);
void bipush(ThreadContext &);
void sipush(ThreadContext &);

#endif // INS_CONSTANTS_H
