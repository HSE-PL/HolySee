#pragma once
#include "Horoutine.hpp"

#include <assert.h>
#include <iostream>
#include <optional>
#include <set>
#include <thread>
#include <utils/log.h>
namespace threads {
  struct ThreadComparator {
    using is_transparent = void;

    bool operator()(const Horoutine& a,
                    const Horoutine& b) const {
      return a.start_sp <= b.start_sp;
    }

    bool operator()(const Horoutine& a,
                    size_t           n) const {
      return a.start_sp < n;
    }

    bool operator()(size_t           n,
                    const Horoutine& b) const {
      return n < b.start_sp;
    }
  };

  class Threads {
  public:
    static Threads& instance();
    std::set<Horoutine, ThreadComparator> pool;
    std::mutex                            _mutex;

    size_t was_sp;

    Threads() : was_sp(0) {}

    void append(void (&func)());

    Horoutine get(size_t sp);
  };
} // namespace threads
