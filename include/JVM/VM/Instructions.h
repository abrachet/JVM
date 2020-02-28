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

#ifndef JVM_VM_INSTRUCTIONS_H
#define JVM_VM_INSTRUCTIONS_H

#include "JVM/VM/ThreadContext.h"
#include <array>
#include <cassert>
#include <cstdint>
#include <functional>
#include <type_traits>

template <typename Func> class FunctionWrapper;

template <typename Ret, typename... Params>
class FunctionWrapper<Ret(Params...)> {
  Ret (*func)(Params...) = nullptr;

public:
  constexpr FunctionWrapper() = default;

  template <typename Func> constexpr FunctionWrapper(Func &&f) : func(f) {}

  constexpr Ret operator()(Params... params) {
    assert(func && "function not initialized");
    return func(params...);
  }
};

using InsT = FunctionWrapper<void(ThreadContext &)>;
extern std::array<InsT, 256> instructions;

namespace Instructions {

struct Instruction {
  const char *name;
  uint8_t value;

  constexpr Instruction(const char *name, uint8_t value)
      : name(name), value(value) {}

  constexpr operator uint8_t() const { return value; }
};

// Constants
constexpr Instruction nop("nop", 0);
constexpr Instruction aconst_null("aconst_null", 1);
constexpr Instruction iconst_m1("iconst_m1", 2);
constexpr Instruction iconst_0("iconst_0", 3);
constexpr Instruction iconst_1("iconst_1", 4);
constexpr Instruction iconst_2("iconst_2", 5);
constexpr Instruction iconst_3("iconst_3", 6);
constexpr Instruction iconst_4("iconst_4", 7);
constexpr Instruction iconst_5("iconst_5", 8);
constexpr Instruction lconst_0("lconst_0", 9);
constexpr Instruction lconst_1("lconst_1", 10);
constexpr Instruction fconst_0("fconst_0", 11);
constexpr Instruction fconst_1("fconst_1", 12);
constexpr Instruction fconst_2("fconst_2", 13);
constexpr Instruction dconst_0("dconst_0", 14);
constexpr Instruction dconst_1("dconst_1", 15);
constexpr Instruction bipush("bipush", 16);
constexpr Instruction sipush("sipush", 17);
constexpr Instruction ldc("ldc", 18);
constexpr Instruction ldc_w("ldc_w", 19);
constexpr Instruction ldc2_2("ldc2_2", 20);

// Loads
constexpr Instruction iload("iload", 21);
constexpr Instruction lload("lload", 22);
constexpr Instruction fload("fload", 23);
constexpr Instruction dload("dload", 24);
constexpr Instruction aload("aload", 25);
constexpr Instruction iload_0("iload_0", 26);
constexpr Instruction iload_1("iload_1", 27);
constexpr Instruction iload_2("iload_2", 28);
constexpr Instruction iload_3("iload_3", 29);
constexpr Instruction fload_0("fload_0", 30);
constexpr Instruction fload_1("fload_1", 31);
constexpr Instruction fload_2("fload_2", 32);
constexpr Instruction fload_3("fload_3", 33);
constexpr Instruction dload_0("dload_0", 34);
constexpr Instruction dload_1("dload_1", 35);
constexpr Instruction dload_2("dload_2", 36);
constexpr Instruction dload_3("dload_3", 37);
constexpr Instruction aload_0("aload_0", 38);
constexpr Instruction aload_1("aload_1", 39);
constexpr Instruction aload_2("aload_2", 40);
constexpr Instruction aload_3("aload_3", 41);
constexpr Instruction iaload("iaload", 42);
constexpr Instruction laload("laload", 43);
constexpr Instruction faload("faload", 44);
constexpr Instruction daload("daload", 45);
constexpr Instruction aaload("aaload", 46);
constexpr Instruction baload("baload", 47);
constexpr Instruction caload("caload", 48);
constexpr Instruction saload("saload", 49);

// Stores
constexpr Instruction istore("istore", 54);
constexpr Instruction lstore("lstore", 55);
constexpr Instruction fstore("fstore", 56);
constexpr Instruction dstore("dstore", 57);
constexpr Instruction astore("astore", 58);
constexpr Instruction istore_0("istore_0", 59);
constexpr Instruction istore_1("istore_1", 60);
constexpr Instruction istore_2("istore_2", 61);
constexpr Instruction istore_3("istore_3", 62);
constexpr Instruction lstore_0("lstore_0", 63);
constexpr Instruction lstore_1("lstore_1", 64);
constexpr Instruction lstore_2("lstore_2", 65);
constexpr Instruction lstore_3("lstore_3", 66);
constexpr Instruction fstore_0("fstore_0", 67);
constexpr Instruction fstore_1("fstore_1", 68);
constexpr Instruction fstore_2("fstore_2", 69);
constexpr Instruction fstore_3("fstore_3", 70);
constexpr Instruction dstore_0("dstore_0", 71);
constexpr Instruction dstore_1("dstore_1", 72);
constexpr Instruction dstore_2("dstore_2", 73);
constexpr Instruction dstore_3("dstore_3", 74);
constexpr Instruction astore_0("astore_0", 75);
constexpr Instruction astore_1("astore_1", 76);
constexpr Instruction astore_2("astore_2", 77);
constexpr Instruction astore_3("astore_3", 78);
constexpr Instruction iastore("iastore", 79);
constexpr Instruction lastore("lastore", 80);
constexpr Instruction fastore("fastore", 81);
constexpr Instruction dastore("dastore", 82);
constexpr Instruction aastore("aastore", 83);
constexpr Instruction bastore("bastore", 84);
constexpr Instruction castore("castore", 85);
constexpr Instruction sastore("sastore", 86);

// Stack
constexpr Instruction pop("pop", 87);
constexpr Instruction pop2("pop2", 88);
constexpr Instruction dup("dup", 89);
constexpr Instruction dup_x1("dup_x1", 90);
constexpr Instruction dup_x2("dup_x2", 91);
constexpr Instruction dup2("dup2", 92);
constexpr Instruction dup2_x1("dup2_x1", 93);
constexpr Instruction dup2_x2("dup2_x2", 94);
constexpr Instruction swap("swap", 95);

// Math
constexpr Instruction iadd("iadd", 96);
constexpr Instruction ladd("ladd", 97);
constexpr Instruction fadd("fadd", 98);
constexpr Instruction dadd("dadd", 99);
constexpr Instruction isub("isub", 100);
constexpr Instruction lsub("lsub", 101);
constexpr Instruction fsub("fsub", 102);
constexpr Instruction dsub("dsub", 103);
constexpr Instruction imul("imul", 104);
constexpr Instruction lmul("lmul", 105);
constexpr Instruction fmul("fmul", 106);
constexpr Instruction dmul("dmul", 107);
constexpr Instruction idiv("idiv", 108);
constexpr Instruction ldiv("ldiv", 109);
constexpr Instruction fdiv("fdiv", 110);
constexpr Instruction ddiv("ddiv", 111);
constexpr Instruction irem("irem", 112);
constexpr Instruction lrem("lrem", 113);
constexpr Instruction frem("frem", 114);
constexpr Instruction drem("drem", 115);
constexpr Instruction ineg("ineg", 116);
constexpr Instruction lneg("lneg", 117);
constexpr Instruction fneg("fneg", 118);
constexpr Instruction dneg("dneg", 119);
constexpr Instruction ishl("ishl", 120);
constexpr Instruction lshl("lshl", 121);
constexpr Instruction ishr("ishr", 122);
constexpr Instruction lshr("lshr", 123);
constexpr Instruction iushr("iushr", 124);
constexpr Instruction lushr("lushr", 125);
constexpr Instruction iand("iand", 126);
constexpr Instruction land("land", 127);
constexpr Instruction ior("ior", 128);
constexpr Instruction lor("lor", 129);
constexpr Instruction ixor("ixor", 130);
constexpr Instruction lxor("lxor", 131);
constexpr Instruction iinc("iinc", 132);

// Conversions
constexpr Instruction i2l("i2l", 133);
constexpr Instruction i2f("i2f", 134);
constexpr Instruction i2d("i2d", 135);
constexpr Instruction l2i("l2i", 136);
constexpr Instruction l2f("l2f", 137);
constexpr Instruction l2d("l2d", 138);
constexpr Instruction f2i("f2i", 139);
constexpr Instruction f2l("f2l", 140);
constexpr Instruction f2d("f2d", 141);
constexpr Instruction d2i("d2i", 142);
constexpr Instruction d2l("d2l", 143);
constexpr Instruction d2f("d2f", 144);
constexpr Instruction i2b("i2b", 145);
constexpr Instruction i2c("i2c", 146);
constexpr Instruction i2s("i2s", 147);

// Comparasions
constexpr Instruction lcmp("lcmp", 148);
constexpr Instruction fcmpl("fcmpl", 149);
constexpr Instruction fcmpg("fcmpg", 150);
constexpr Instruction dcmpl("dcmpl", 151);
constexpr Instruction dcmpg("dcmpg", 152);
constexpr Instruction ifeq("ifeq", 153);
constexpr Instruction ifne("ifne", 154);
constexpr Instruction iflt("iflt", 155);
constexpr Instruction ifge("ifge", 156);
constexpr Instruction ifgt("ifgt", 157);
constexpr Instruction ifle("ifle", 158);
constexpr Instruction if_icmpeq("if_icmpeq", 159);
constexpr Instruction if_icmpne("if_icmpne", 160);
constexpr Instruction if_icmplt("if_icmplt", 161);
constexpr Instruction if_icmpge("if_icmpge", 162);
constexpr Instruction if_icmpgt("if_icmpgt", 163);
constexpr Instruction if_icmple("if_icmple", 164);
constexpr Instruction if_acmpeq("if_acmpeq", 165);
constexpr Instruction if_acmpne("if_acmpne", 166);

// Control
constexpr Instruction goto_("goto_", 167);
constexpr Instruction jsr("jsr", 168);
constexpr Instruction ret("ret", 169);
constexpr Instruction tableswitch("tableswitch", 170);
constexpr Instruction lookupswitch("lookupswitch", 171);
constexpr Instruction ireturn("ireturn", 172);
constexpr Instruction lreturn("lreturn", 173);
constexpr Instruction freturn("freturn", 174);
constexpr Instruction dreturn("dreturn", 175);
constexpr Instruction areturn("areturn", 176);
constexpr Instruction return_("return_", 177);

// References
constexpr Instruction getstatic("getstatic", 178);
constexpr Instruction putstatic("putstatic", 179);
constexpr Instruction getfield("getfield", 180);
constexpr Instruction putfield("putfield", 181);
constexpr Instruction invokevirtual("invokevirtual", 182);
constexpr Instruction invokespecial("invokespecial", 183);
constexpr Instruction invokestatic("invokestatic", 184);
constexpr Instruction invokeinterface("invokeinterface", 185);
constexpr Instruction invokedynamic("invokedynamic", 186);
constexpr Instruction new_("new_", 187);
constexpr Instruction newarray("newarray", 188);
constexpr Instruction anewarray("anewarray", 189);
constexpr Instruction arraylength("arraylength", 190);
constexpr Instruction athrow("athrow", 191);
constexpr Instruction checkcast("checkcast", 192);
constexpr Instruction instanceof ("instanceof", 193);
constexpr Instruction monitorenter("monitorenter", 194);
constexpr Instruction monitorexit("monitorexit", 195);

// Extended
constexpr Instruction wide("wide", 196);
constexpr Instruction multianewarray("multianewarray", 197);
constexpr Instruction ifnull("ifnull", 198);
constexpr Instruction ifnonnull("ifnonnull", 199);
constexpr Instruction goto_w("goto_w", 200);
constexpr Instruction jsr_w("jsr_w", 201);

// Reserved
constexpr Instruction breakpoint("breakpoint", 202);
constexpr Instruction impdep1("impdep1", 254);
constexpr Instruction impdep2("impdep2", 255);

} // namespace Instructions

#endif // JVM_VM_INSTRUCTIONS_H
