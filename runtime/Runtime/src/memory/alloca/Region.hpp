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

  Region(size_t start_region, size_t count_items,
         size_t t_size);
};