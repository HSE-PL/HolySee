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
  logezhe << "calling GC\n";
  for (auto i = 0; i < threads::Threads::instance().count(); --i)
    threads::Threads::instance().tracing_.acquire();
  logezhe << "tracing end\n";

  cleaning();

  logezhe << "end cleaning\n";
  sp::on();
  threads::Threads::instance().cleaning_.release(
      static_cast<std::ptrdiff_t>(threads::Threads::instance().count()));

  logezhe << "STW end, time: " << std::fixed << std::setprecision(3)
          << 1000 * static_cast<double>(clock() - s) / CLOCKS_PER_SEC << "ms \n";
  // allocator_.dump();
}

auto GarbageCollector::tracing() -> void {
  while (true) {
    ref ptr;
    if (auto result = queue_for_marked_.try_pop(ptr); result && how_many_ref(ptr))
      marking(ptr);
    else {
      logezhe << "tracing: ohh, stack is empty(((\n";
      if (!threads::Threads::instance().count_of_working_threads_)
        throw std::runtime_error("fantom working thread");
      logezhe << "with me, count_of_working_threads = "
              << threads::Threads::instance().count_of_working_threads_ << "\n";
      if (!--threads::Threads::instance().count_of_working_threads_)
        break;
      while (queue_for_marked_.empty())
        if (!threads::Threads::instance().count_of_working_threads_)
          return;
      ++threads::Threads::instance().count_of_working_threads_;
    }
  }
}

auto GarbageCollector::make_root_and_tracing(ref ssp) -> void {
  logezhe << "calling make_root_and_tracing\n";

  auto hrtptr = threads::Threads::instance().get(ssp);
  logezhe << hrtptr.start_sp << ":\n.\n.\n.\n"
          << ssp << "\n"
          << "summary diff: " /*<< std::hex*/
          << hrtptr.start_sp - ssp << "\n";

  assert(ssp < hrtptr.start_sp);
  for (ref sp = ssp; sp <= hrtptr.start_sp; sp += 8) {
    auto ptr = *reinterpret_cast<ref*>(sp);
    logezhe << "make_root_and_tracing: on stack: " << ptr << "\n";
    if (ref_in_heap(ptr)) {
      logezhe << "find " << ptr << " in root\n";
      // memory_.fetch_sub(how_many_ref(ptr));
      queue_for_marked_.push(ptr);
    }
  }
  logezhe << "thread completed root\n";

  tracing();

  logezhe << "thread completed tracing\n";

  threads::Threads::instance()
      .tracing_.release(); // tell gc we`re done! (for start cleaning)

  logezhe << "thread end\n";
}