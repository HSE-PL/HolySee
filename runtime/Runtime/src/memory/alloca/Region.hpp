#pragma once
#include <vector>

template <typename T>
concept have_size = requires(T item) {
  { item.size } -> std::convertible_to<size_t>;
};

template <have_size T>
class Region {

public:
  const size_t start;
  const size_t count;
  const size_t size;

  const size_t    t_size;
  std::vector<T*> items;
  std::vector<T*> pull;

  Region(size_t start_region, size_t count_items, size_t t_size,
         size_t region_tier)
      : start(start_region), count(count_items), size(count * t_size),
        t_size(t_size), items() {
    for (int i = 0; i < count_items; i++) {
      T* a = new T(t_size, start_region + t_size * i, region_tier);
      pull.push_back(a);
      items.push_back(a);
    }
  }
};