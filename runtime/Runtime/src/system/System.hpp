#pragma once
#include "utils/defines.h"
#include <stddef.h>

namespace sys {
  fn salloc(size_t size)->void*;
} // namespace sys