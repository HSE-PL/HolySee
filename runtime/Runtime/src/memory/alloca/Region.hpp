#pragma once
#include "utils/defines.h"
#include <vector>

#include <memory/ThreadSafeVector.hpp>
template <typename T>
concept have_size = requires(T item) {
  { item.size } -> std::convertible_to<size_t>;
};

template <have_size T>
class Region {

  std::vector<T*>      items_;
  ThreadSafeVector<T*> pull_;

public:
  std::mutex   mutex_;
  const ref    start;
  const size_t count;
  const size_t size;

  const size_t t_size;

  bool have_empty;

  Region(ref start_region, size_t count_items, size_t t_size, size_t region_tier)
      : start(start_region), count(count_items), size(count * t_size), t_size(t_size),
        have_empty(false), items_() {
    for (int i = 0; i < count_items; i++) {
      T* a = new T(t_size, start_region + t_size * i, region_tier);
      pull_.push(a);
      items_.push_back(a);
    }
  }

  void push(T* t) {
    pull_.push(t);
  }

  void pop() {
    pull_.pop();
  }

  T* back() {
    return pull_.back();
  }

  T* operator[](size_t index) const {
    return items_[index];
  }
};