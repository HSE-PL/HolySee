#pragma once
#include "memory/alloca/Arena.hpp"
#include "utils/defines.h"
#include <stddef.h>
namespace sys {
  auto salloc(size_t size, size_t tier) -> void*;
  auto sfree(Arena*) -> void;
  auto sreserve(size_t size) -> void;
} // namespace sys