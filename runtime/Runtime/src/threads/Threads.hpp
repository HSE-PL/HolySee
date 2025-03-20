#pragma once
#include "Horoutine.hpp"
#include "ThreadSafeCounter.hpp"

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
    std::mutex                            mutex_;

    bool releaseIfAll(std::counting_semaphore<>& sem) {
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

    void waitEndSp();
    void waitEndRooting();

    std::counting_semaphore<> root_was_collected_;
    std::counting_semaphore<> rooting_;
    std::counting_semaphore<> marking_;

    Threads() : counter_(0), sp_(0), rooting_(0), marking_(0), root_was_collected_(0) {
    }

    void append(void (&func)());

    size_t count() {
      return pool_.size();
    }

    Horoutine get(size_t sp);
  }; // namespace threads
} // namespace threads
