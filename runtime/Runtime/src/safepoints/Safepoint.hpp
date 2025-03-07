#pragma once
#include <stddef.h>

namespace sp {
  inline size_t pagesize = 1 << 12;
  inline void*  spd;

  inline void change(int prot);

  inline void off();

  inline void on();

  inline void init(void** spdptr);
} // namespace sp
