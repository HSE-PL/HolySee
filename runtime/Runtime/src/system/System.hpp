#pragma once
#include "memory/alloca/Arena.hpp"
#include "utils/defines.h"
#include <stddef.h>
namespace sys {
  auto alloc(size_t size, size_t tier) -> Arena*;
  auto free(Arena*) -> void;
  auto reserve(size_t size) -> void;
} // namespace sys