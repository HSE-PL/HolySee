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

template <typename T>
struct inteval {
  ref first;
  ref last;

  inteval(T* i1, T* i2) : first(reinterpret_cast<ref>(i1)), last(reinterpret_cast<ref>(i2)) {
  }

  fn correct(T* i)->bool {
    let p = reinterpret_cast<ref>(i);
    if (p < first)
      return false;
    if (p > last)
      return false;
    let offset = p - first;
    return !(offset % sizeof(T));
  }
};