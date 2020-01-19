
#ifndef INC_INSTRUCTION_H
#define INC_INSTRUCTION_H

#include <cstdint>

static inline bool previousInsIs(const void *pc, uint8_t ins) {
  return reinterpret_cast<const uint8_t *>(pc)[-1] == ins;
}

#endif // INC_INSTRUCTION_H
