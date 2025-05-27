#pragma once
#include "memory/alloca/Arena.hpp"
#include "utils/defines.h"
#include <stddef.h>
namespace sys {
  auto aalloc(Arena* a) -> void;
  auto free(Arena*) -> void;
  auto reserve(size_t size) -> void;
} // namespace sys