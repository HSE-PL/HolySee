#pragma once
#include <cstddef>
#include <sys/types.h>
#define guard(mutex) std::lock_guard _(mutex)
#define __holy_def_args                                                        \
  u_int64_t _1, u_int64_t _2, u_int64_t _3, u_int64_t _4, u_int64_t _5
#define __holy_args _1, _2, _3, _4, _5
#define __holy_empty_args 0, 0, 0, 0, 0
typedef unsigned long long ref;
static void*               __dso_handle = nullptr;

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
