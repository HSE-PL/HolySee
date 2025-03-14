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

template <typename T>
concept ItemForHeap = requires(T item) {
  {
    item.key_for_heap()
  } -> std::convertible_to<size_t>;
  {
    item.uniq_for_heap()
  } -> std::convertible_to<size_t>;
};

template <typename T>
struct Comparator {
  using is_transparent = void;

  bool operator()(const T* a, const T* b) const {
    log << "check " << a << " ? " << b << "\n";
    auto cond =
        a->key_for_heap() != b->key_for_heap()
            ? a->key_for_heap() <
                  b->key_for_heap()
            : a->uniq_for_heap() <
                  b->uniq_for_heap();
    log << "end of check\n";
    return cond;
  }

  bool operator()(const T* a, size_t n) const {
    log << a << "\n";
    return a->key_for_heap() < n;
  }

  bool operator()(size_t n, const T* b) const {
    log << b << "\n";
    return n < b->key_for_heap();
  }
};

template <ItemForHeap T>
class Heap {
public:
  virtual ~Heap() = default;
  Heap()          = default;

  std::set<T*, Comparator<T>> keys;
  std::mutex                  _mutex;

  T* get_min_more_then(size_t n) {
    _mutex.lock();
    auto el = keys.lower_bound(n);
    _mutex.unlock();
    assert(el != keys.end());
    return *el;
  }

  void append(T* a) {
    log << "try to get _mutex, ";
    _mutex.lock();
    log << "call insert int thread: "
        << std::this_thread::get_id() << "\n";
    keys.insert(a);
    log << "insert end, ";
    _mutex.unlock();
    log << "_mutex unlock\n";
  }

  void del(T* a) {
    _mutex.lock();
    keys.erase(a);
    _mutex.unlock();
  }

  // 4 debug
  void printSetTree(
      typename std::set<T*>::iterator it,
      typename std::set<T*>::iterator end,
      int                             depth = 0) {
    if (it == end)
      return;

    auto right = std::next(it);
    printSetTree(right, end, depth + 1);

    for (int i = 0; i < depth; ++i)
      log << "    ";
    log << (*it)->uniq_for_heap() << ":"
        << (*it)->key_for_heap() << "\n";

    if (it != end)
      printSetTree(it, it, depth + 1);
  }

  void print() {
    log << "=====>\n";
    printSetTree(keys.begin(), keys.end());
    log << "<=====\n";
  }
};
