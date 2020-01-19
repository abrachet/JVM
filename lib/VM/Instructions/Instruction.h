
#include <cstdint>

static inline bool previousInsIs(const void *pc, uint8_t ins) {
  return reinterpret_cast<const uint8_t *>(pc)[-1] == ins;
}
