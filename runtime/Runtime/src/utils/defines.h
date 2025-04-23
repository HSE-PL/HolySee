#pragma once
#include <cstddef>
#define guard(mutex) std::lock_guard _(mutex)
#define fn auto
#define let auto
typedef unsigned long long ref;

struct instance {
  size_t      size;
  const char* name;
  int         have_ref;
};