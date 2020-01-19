
#ifndef INC_CONSTANTS_CONSTANTS_H
#define INC_CONSTANTS_CONSTANTS_H

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

#endif // INC_CONSTANTS_CONSTANTS_H
