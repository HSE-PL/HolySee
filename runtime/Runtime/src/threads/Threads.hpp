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
    size_t counter_;

    std::counting_semaphore<> sp_;

    std::set<Horoutine, ThreadComparator> pool_;

    std::mutex mutex_;

    fn releaseIfAll(std::counting_semaphore<>& sem)->bool {
      guard(mutex_);
      if (count() == ++counter_) {
        counter_ = 0;
        sem.release(count());
        return true;
      }
      return false;
    }

  public:
    static Threads& instance();

    fn wait_end_sp()->void;
    fn wait_end_tracing()->void;

    std::counting_semaphore<> tracing_was_complete_;
    std::counting_semaphore<> rooting_;
    std::counting_semaphore<> marking_;

    std::atomic_uint_fast64_t count_of_working_threads_;
    Threads() : counter_(0), sp_(0), rooting_(0), marking_(0), tracing_was_complete_(0) {
    }

    fn append(void (&func)())->void;

    fn count()->size_t {
      return pool_.size();
    }

    fn get(size_t sp)->Horoutine;
  }; // namespace threads
} // namespace threads
