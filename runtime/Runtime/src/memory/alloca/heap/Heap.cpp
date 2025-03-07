#include "Heap.hpp"

template <ItemForHeap T>
T Heap<T>::get_min_more_then(size_t n) {
  return keys.lower_bound(n);
}

template <ItemForHeap T>
void Heap<T>::append(T a) {
  keys.insert(a);
}

template <ItemForHeap T>
void Heap<T>::replace(T old_el, T new_el) {
  keys.erase(old_el);
  keys.insert(new_el);
}
