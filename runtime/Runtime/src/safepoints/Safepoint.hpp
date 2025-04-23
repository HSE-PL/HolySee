#pragma once
#include <stddef.h>

namespace sp {
  inline static const size_t pagesize = 1 << 12;
  inline void*               spd;

  void change(int prot);

  void off();

  void on();

  void init(void** spdptr);
} // namespace sp
