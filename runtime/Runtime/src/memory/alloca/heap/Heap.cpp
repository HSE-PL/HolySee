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
void Heap<T>::decrease_key(T element, size_t new_key) {
  keys.erase(element);
  element.key_for_heap()
}
