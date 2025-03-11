#pragma once
#include <concepts>
#include <cstddef>
#include <iostream>
#include <set>
#include <stdexcept>

template <typename T>
concept ItemForHeap = requires(T* item) {
  {
    item->key_for_heap()
  } -> std::convertible_to<size_t>;
  {
    item->uniq_for_heap()
  } -> std::convertible_to<size_t>;
};
template <typename T>
struct AllocSet {};
template <typename T>
struct Comparator {
  using is_transparent = void;

  bool operator()(const T* a, const T* b) const {
    std::cout << "check " << a << " "
              << a->uniq_for_heap() << " ? " << b
              << " " << b->uniq_for_heap()
              << std::endl;
    auto res =
        a->key_for_heap() != b->key_for_heap()
            ? a->key_for_heap() <
                  b->key_for_heap()
            : a->uniq_for_heap() <
                  b->uniq_for_heap();
    std::cout << "checkend\n";
    return res;
  }

  bool operator()(const T* a, size_t n) const {
    std::cout << a << std::endl;
    return a->key_for_heap() < n;
  }

  bool operator()(size_t n, const T* b) const {
    std::cout << b << std::endl;
    return n < b->key_for_heap();
  }
};

template <ItemForHeap T>
class Heap {
public:
  virtual ~Heap() = default;
  Heap()          = default;

  std::set<T*, Comparator<T>> keys;

  T* get_min_more_then(size_t n) {
    auto el = keys.lower_bound(n);
    if (el == keys.end())
      throw std::runtime_error(
          "very big object (size: " +
          std::to_string(n) + ")");
    return *el;
  }

  void append(T* a) {
    std::cout << "call insert\n"
              << a->uniq_for_heap() << ":"
              << a->key_for_heap() << std::endl;
    print();
    if (keys.contains(a))
      throw "oisdfj";
    keys.insert(a); // <---- sigfault

    std::cout
        << "insert end\n"; // i dont see this line
                           // in logs
  }

  void del(T* a) { keys.erase(a); }

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
      std::cout << "    ";
    std::cout << (*it)->uniq_for_heap() << ":"
              << (*it)->key_for_heap() << "\n";

    if (it != end)
      printSetTree(it, it, depth + 1);
  }

  void print() {
    std::cout << "=====>\n";
    printSetTree(keys.begin(), keys.end());
    std::cout << "<=====\n";
  }
};
