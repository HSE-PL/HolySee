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
    Threads() : counter_(0), sp_(0), tracing_(0), cleaning_(0) {
    }

    auto append(void (&func)(__holy_def_args), __holy_def_args) -> void {
      guard(mutex_);

      std::binary_semaphore s(0);
      std::binary_semaphore e(0);

      const auto thread_is_died = new bool(false);


      const auto thread =
          std::make_shared<std::thread>([&, thread_is_died, __holy_args]() {
            s.acquire();
            // std::cout << "start\n";
            e.release();
            func(__holy_args);
            // std::cout << "end\n";
            *thread_is_died = true;
            return;
          });

      const auto pthread = thread->native_handle();

      pthread_attr_t attr;

      void*  stack_addr;
      size_t stack_size;
      pthread_getattr_np(pthread, &attr);
      pthread_attr_getstack(&attr, &stack_addr, &stack_size);
      pthread_attr_destroy(&attr);

      auto hrptr = Horoutine{
          thread, reinterpret_cast<size_t>(stack_addr) + stack_size - 4592,
          thread_is_died}; // hehehe

      pool_.insert(hrptr);
      s.release();
      e.acquire();
      // std::cout << "thrd append\n";
    }

    auto deappend(ref sp) -> void;

    auto count() const -> size_t {
      return pool_.size();
    }

    auto get(size_t sp) -> Horoutine;

    auto begin() const {
      return pool_.begin();
    }

    auto end() const {
      return pool_.end();
    }

  }; // namespace threads
} // namespace threads
