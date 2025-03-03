#pragma once
#include <concepts>
#include <cstddef>
#include <set>

template <typename T>
concept ItemForHeap = requires(T item) {
  { item.key_for_heap() } -> std::convertible_to<size_t>;
};

template <ItemForHeap T>
class Heap {
  std::set < T, struct Comparator {
    bool operator()(T a, T b) {
      return a.key_for_heap < b.key_for_heap;
    }
  } > keys;

protected:
  Heap() = default;

  static T get_min_more_then(size_t n);

  static void append(T a);

  static void replace(T old_el, T new_el);
};
