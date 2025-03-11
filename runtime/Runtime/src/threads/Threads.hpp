#pragma once
#include <iostream>
#include <set>
#include <thread>
namespace threads {

  static size_t hash_id(
      const std::unique_ptr<std::thread>& thrd) {
    return std::hash<std::thread::id>{}(
        thrd->get_id());
  }

  struct ThreadComparator {
    using is_transparent = void;

    bool operator()(
        const std::unique_ptr<std::thread>& a,
        const std::unique_ptr<std::thread>& b)
        const {
      return hash_id(a) < hash_id(b);
    }

    bool operator()(
        const std::unique_ptr<std::thread>& a,
        size_t n) const {
      return hash_id(a) < n;
    }

    bool
    operator()(size_t n,
               const std::unique_ptr<std::thread>&
                   b) const {
      return n < hash_id(b);
    }
  };

  class Threads {
    std::set<std::unique_ptr<std::thread>,
             ThreadComparator>
        pool;

  public:
    size_t was_sp;

    Threads() : was_sp(0) {}

    void append(void (&func)()) {
      std::cout << "try to append thrd\n";

      std::thread t(func);
      // pool.insert(std::make_unique<std::thread>(
      //     std::move(thrd)));
      std::cout << "thrd append\n";
      t.join();
    }
  };
} // namespace threads
