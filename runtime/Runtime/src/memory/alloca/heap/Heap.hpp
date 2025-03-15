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
};

template <typename T>
struct Comparator {
  using is_transparent = void;

  bool operator()(const T* a, const T* b) const {
    log << "check " << a << " ? " << b << "\n";
    auto cond = a->key_for_heap() != b->key_for_heap() ? a->key_for_heap() < b->key_for_heap()
                                                       : a->uniq_for_heap() < b->uniq_for_heap();
    log << "end of check\n";
    return cond;
  }

  bool operator()(const T* a, ref n) const {
    log << a << "\n";
    return a->key_for_heap() < n;
  }

  bool operator()(ref n, const T* b) const {
    log << b << "\n";
    return n < b->key_for_heap();
  }
};

template <ItemForHeap T>
class Heap {
  std::mutex mutex_;

protected:
  std::set<T*, Comparator<T>> keys;

public:
  virtual ~Heap() = default;
  Heap()          = default;


  T* get_min_more_then(ref n) {
    mutex_.lock();
    auto el = keys.lower_bound(n);
    mutex_.unlock();
    assert(el != keys.end());
    return *el;
  }

  void append(T* a) {
    log << "try to get _mutex, ";
    mutex_.lock();
    log << "call insert int thread: " << std::this_thread::get_id() << "\n";
    keys.insert(a);
    log << "insert end, ";
    mutex_.unlock();
    log << "_mutex unlock\n";
  }

  void del(T* a) {
    mutex_.lock();
    keys.erase(a);
    mutex_.unlock();
  }

  // 4 debug
  void printSetTree(typename std::set<T*>::iterator it, typename std::set<T*>::iterator end,
                    int depth = 0) {
    if (it == end)
      return;

    auto right = std::next(it);
    printSetTree(right, end, depth + 1);

    for (int i = 0; i < depth; ++i)
      log << "    ";
    log << (*it)->uniq_for_heap() << ":" << (*it)->key_for_heap() << "\n";

    if (it != end)
      printSetTree(it, it, depth + 1);
  }

  void print() {
    log << "=====>\n";
    printSetTree(keys.begin(), keys.end());
    log << "<=====\n";
  }
};
