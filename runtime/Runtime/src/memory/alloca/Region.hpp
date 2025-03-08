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

  const size_t   t_size;
  std::vector<T> items;

  size_t count_empty;
  Region(size_t start_region, size_t count_items, size_t t_size,
         size_t region_tier)
      : start(start_region), count(count_items), size(count * t_size),
        t_size(t_size), items(), count_empty(0) {
    for (int i = 0; i < count_items;
         items.push_back(T(t_size, start_region + t_size * i++, region_tier)))
      ;
  }
};