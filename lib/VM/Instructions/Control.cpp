
#include "Control.h"

// TODO: monitorexit on syncronized methods.
template <size_t TypeWidth> void treturn(ThreadContext &tc) {
  uint64_t arg = tc.stack.pop<TypeWidth>();
  Frame frame = tc.popFrame();
  tc.pc = frame.returnAddress;
  tc.stack.push<TypeWidth>(arg);
}

void ireturn(ThreadContext &tc) { return treturn<1>(tc); }
