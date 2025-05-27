#pragma once
#include <cstddef>
#define guard(mutex) std::lock_guard _(mutex)
typedef unsigned long long ref;

static void* __dso_handle = nullptr;

struct instance {
  size_t      size;
  const char* name;
  long        have_ref;
};

template <typename T>
struct inteval {
  ref first;
  ref last;

  inteval(T* i1, T* i2)
      : first(reinterpret_cast<ref>(i1)), last(reinterpret_cast<ref>(i2)) {
  }

  auto correct(T* i) -> bool {
    auto p = reinterpret_cast<ref>(i);
    if (p < first)
      return false;
    if (p > last)
      return false;
    auto offset = p - first;
    return !(offset % sizeof(T));
  }
};

auto constexpr operator"" _page(unsigned long long n) -> size_t {
  return n << 12;
}

auto constexpr operator"" _ref(unsigned long long n) -> size_t {
  return n << 3;
}
