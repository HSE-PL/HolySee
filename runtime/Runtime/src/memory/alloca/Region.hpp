#pragma once
#include "utils/defines.h"
#include <assert.h>
#include <atomic>
#include <source_location>
#include <utils/FuckTBB.hpp>
#include <vector>

template <typename T>
concept have_size = requires(T item) {
  { item.size } -> std::convertible_to<size_t>;
};

template <have_size T>
class Region {
public:
  std::vector<T*> items_;

  ThreadSafeVector<T*> slots_;

  const ref    start;
  const size_t size;
  const size_t t_size;

  Region(ref start_region, size_t count_items, size_t t_size,
         size_t region_tier)
      : start(start_region), size(count_items * t_size), t_size(t_size),
        items_() {

    for (int i = 0; i < count_items; i++) {
      T* a = new T(t_size, start_region + t_size * i, region_tier);

      slots_.push(a);
      items_.push_back(a);
    }
  }

  auto push(T* t) -> void {
    slots_.push(t);
  }

  auto empty() -> bool {
    return slots_.empty();
  }

  auto pop() {
    return slots_.pop();
  }

  T* operator[](size_t index) const {
    return items_[index];
  }
};