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
  std::set<T, struct Comparator {
    bool operator()(T a, T b) {
      return a.key_for_heap < b.key_for_heap;
    }}> keys;

protected:
  Heap() = default;

  static T get_min_more_then(size_t n) {
    return keys.lower_bound(n);
  }

  static void append(T a) {
    keys.insert(a);
  }

  static void decrease_key(T element) {
    keys.erase(element);
    keys.insert(element);
  }
};
