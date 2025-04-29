#pragma once
#include "memory/alloca/Arena.hpp"
#include "utils/defines.h"
#include <stddef.h>
namespace sys {
  fn salloc(size_t size)->void*;
  fn holy(Arena*)->void;
} // namespace sys