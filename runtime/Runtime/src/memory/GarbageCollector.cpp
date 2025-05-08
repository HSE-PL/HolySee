#include "GarbageCollector.hpp"
#include "Runtime.hpp"
#include "safepoints/Safepoint.hpp"
#include "threads/Threads.hpp"
#include "utils/log.h"
#include <cassert>
#include <cstddef>
#include <iomanip>

GarbageCollector::GarbageCollector(instance* first, instance* last)
    : inteval_(first, last) {
}

auto GarbageCollector::GC() -> void {
  auto s = clock();
  for (auto i = 0; i < threads::Threads::instance().count(); ++i)
    threads::Threads::instance().tracing_.acquire();

  std::atomic_thread_fence(std::memory_order_acquire);
  cleaning();

  std::atomic_thread_fence(std::memory_order_release);

  ++num_of_cleaning;
  sp::on();
  threads::Threads::instance().cleaning_.release(
      static_cast<std::ptrdiff_t>(threads::Threads::instance().count()));

  std::cout << "STW end, time: " << std::fixed << std::setprecision(3)
            << 1000 * static_cast<double>(clock() - s) / CLOCKS_PER_SEC
            << "ms \n";
}

auto GarbageCollector::tracing() -> void {
  while (true) {
    if (auto ptr = stack_for_marked_.pop().value_or(0); ptr)
      marking(ptr);
    else {
      if (!threads::Threads::instance().count_of_working_threads_)
        throw std::runtime_error("fantom working thread");

      if (!--threads::Threads::instance().count_of_working_threads_) {
        stack_for_marked_.clear();
        break;
      }

      while (stack_for_marked_.empty())
        if (!threads::Threads::instance().count_of_working_threads_)
          return;
      ++threads::Threads::instance().count_of_working_threads_;
    }
  }
}

auto GarbageCollector::make_root_and_tracing(ref ssp) -> void {
  auto hrtptr = threads::Threads::instance().get(ssp);
  logezhe << hrtptr.start_sp << ":\n.\n.\n.\n"
          << ssp << "\n"
          << "summary diff: " /*<< std::hex*/
          << hrtptr.start_sp - ssp << "\n";

  assert(ssp < hrtptr.start_sp);
  for (ref sp = ssp; sp <= hrtptr.start_sp; sp += 1_ref) {
    auto ptr = *reinterpret_cast<ref*>(sp);
    logezhe << "make_root_and_tracing: on stack: " << ptr << "\n";
    if (ref_is_real(ptr)) {
      logezhe << "find " << ptr << " in root\n";
      stack_for_marked_.push(ptr);
    }
  }
  tracing();

  std::atomic_thread_fence(std::memory_order_release);
  threads::Threads::instance()
      .tracing_.release(); // tell gc we`re done! (for start cleaning)
}