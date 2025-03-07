#pragma once
#include <concepts>
#include <cstddef>
#include <set>

template <typename T>
concept ItemForHeap = requires(T item) {
  {
    item.key_for_heap()
  } -> std::convertible_to<size_t>;
};

template <typename T>
struct Comparator {
  bool operator()(const T& a, const T& b) const {
    return a.key_for_heap < b.key_for_heap;
  }
};

template <ItemForHeap T>
class Heap {
public:
  virtual ~Heap() = default;

protected:
  std::set<T, Comparator<T>> keys;

  Heap() = default;

  T get_min_more_then(size_t n);

  void append(T a);

  void replace(T old_el, T new_el);
};
