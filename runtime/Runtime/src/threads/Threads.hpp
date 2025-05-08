#pragma once
#include "Horoutine.hpp"

#include <assert.h>
#include <iostream>
#include <memory/GarbageCollector.hpp>
#include <optional>
#include <set>
#include <thread>
#include <utils/log.h>

namespace threads {
  struct ThreadComparator {
    using is_transparent = void;

    bool operator()(const Horoutine& a, const Horoutine& b) const {
      return a.start_sp <= b.start_sp;
    }

    bool operator()(const Horoutine& a, size_t n) const {
      return a.start_sp < n;
    }

    bool operator()(size_t n, const Horoutine& b) const {
      return n < b.start_sp;
    }
  };

  class Threads {
    std::atomic_uint_fast64_t counter_;

    std::counting_semaphore<> sp_;

    std::set<Horoutine, ThreadComparator> pool_;

    std::mutex mutex_;

    auto releaseIfAll(std::counting_semaphore<>& sem) -> void {
      guard(mutex_);
      if (count() == ++counter_) {
        counter_.store(0);
        sem.release(count());
      }
    }

  public:
    static Threads& instance();

    auto wait_end_sp() -> void;
    auto wait_end_tracing() -> void;

    std::counting_semaphore<> tracing_;
    std::counting_semaphore<> cleaning_;

    std::atomic_uint_fast64_t count_of_working_threads_;
    Threads() : counter_(0), sp_(0), cleaning_(0), tracing_(0) {
    }

    auto append(void (&func)()) -> void;

    auto count() -> size_t {
      return pool_.size();
    }

    auto get(size_t sp) -> Horoutine;
  }; // namespace threads
} // namespace threads
