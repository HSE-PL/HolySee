#pragma once
#include "utils/log.h"
#include <cassert>
#include <concepts>
#include <cstddef>
#include <iostream>
#include <mutex>
#include <set>
#include <stdexcept>
#include <thread>
#include <utils/defines.h>

template <typename T>
concept ItemForHeap = requires(T item) {
  { item.key_for_heap() } -> std::convertible_to<size_t>;
  { item.uniq_for_heap() } -> std::convertible_to<size_t>;
  { item.kill() };
};

template <typename T>
struct Comparator {
  using is_transparent = void;

  bool operator()(const T* a, const T* b) const {
    return a->key_for_heap() != b->key_for_heap()
               ? a->key_for_heap() < b->key_for_heap()
               : a->uniq_for_heap() < b->uniq_for_heap();
  }

  bool operator()(const T* a, ref n) const {
    return a->key_for_heap() < n;
  }

  bool operator()(ref n, const T* b) const {
    return n < b->key_for_heap();
  }
};

template <ItemForHeap T>
class Heap {
  std::mutex mutex_;

public:
  virtual ~Heap() = default;
  Heap()          = default;

  std::set<T*, Comparator<T>> keys;

  T* get_min_more_then(ref n) {
    guard(mutex_);
    // print();
    return keys.lower_bound(n) != keys.end() ? *keys.lower_bound(n) : nullptr;
  }

  void append(T* a) {
    guard(mutex_);
    log << "call insert in thread: " << std::this_thread::get_id() << "\n";
    keys.insert(a);
    log << "insert end, ";
  }

  void del(T* a) {
    guard(mutex_);
    log << a << "\n";
    keys.erase(a);
  }

  // 4 debug
  fn printSetTree(typename std::set<T*>::iterator it, typename std::set<T*>::iterator end,
                  int depth = 0)
      ->void {
    if (it == end)
      return;

    auto right = std::next(it);
    printSetTree(right, end, depth + 1);

    for (int i = 0; i < depth; ++i)
      log << "    ";
    log << std::hex << (*it)->uniq_for_heap() << ":" << (*it)->key_for_heap() << "\n";

    if (it != end)
      printSetTree(it, it, depth + 1);
  }

  fn print() {
    log << "=====>\n";
    printSetTree(keys.begin(), keys.end());
    log << "<=====\n";
  }
};
