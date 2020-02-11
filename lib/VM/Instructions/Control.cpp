
#include "Control.h"

template <size_t TypeWidth> void treturn(ThreadContext &tc) {
  if (tc.currentFrame().syncronizedMethod)
    tc.getLoadedClass().second.monitor.unlock();
  uint64_t arg = tc.stack.pop<TypeWidth>();
  Frame frame = tc.popFrame();
  tc.pc = frame.returnAddress;
  tc.stack.push<TypeWidth>(arg);
}

void ireturn(ThreadContext &tc) { return treturn<1>(tc); }
