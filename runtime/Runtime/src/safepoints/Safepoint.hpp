#pragma once
#include <stddef.h>

namespace sp {
  inline void* spd;

  inline bool sp = false;

  void change(int prot);

  void off();

  void on();

  void init(void** spdptr);
} // namespace sp
