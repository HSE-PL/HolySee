#include "Safepoint.hpp"
#include "system/System.hpp"
#include <sys/mman.h>

namespace sp {
  inline void change(int prot) {
    mprotect(spd, pagesize, prot);
  }

  inline void off() {
    change(PROT_NONE);
  }

  inline void on() {
    change(PROT_READ);
  }

  inline void init(void** spdptr) {
    spd = sys::salloc(pagesize);

    *spdptr = spd;
  }
}; // namespace sp