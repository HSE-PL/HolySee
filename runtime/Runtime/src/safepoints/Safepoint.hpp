#pragma once
#include <atomic>
#include <stddef.h>

namespace sp {
  inline void* spd;

  inline std::atomic_bool sp = false;

  void change(int prot);

  void off();

  void on();

  void init(void** spdptr);
} // namespace sp
