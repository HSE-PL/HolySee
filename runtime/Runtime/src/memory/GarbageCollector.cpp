#include "GarbageCollector.hpp"
#include "Runtime.hpp"
#include "safepoints/Safepoint.hpp"
#include "threads/Threads.hpp"
#include "utils/log.h"
#include <cassert>
#include <cstddef>
#include <iomanip>

GarbageCollector::GarbageCollector(ref start_heap, size_t size_heap, instance* first,
                                   instance* last)
    : checked_(start_heap, start_heap + size_heap), allocator_(start_heap, size_heap),
      inteval_(first, last) {
}

fn GarbageCollector::alloc(instance* inst)->ref {
  let ptr = allocator_.alloc(inst->size + 8);
  while (!ptr) {
    log << "AOM!!!!!!!!!!!!!!\n";
    sp::off();
    gogc(reinterpret_cast<ref>(&ptr));
    ptr = allocator_.alloc(inst->size + 8);
  }
  log << "gc alloc: end\n";
  return ptr;
}

fn GarbageCollector::GC()->void {
  let s = clock();
  log << "calling GC\n";
  for (let i = 0; i < threads::Threads::instance().count(); --i)
    threads::Threads::instance().tracing_.acquire();
  log << "tracing end\n";

  cleaning();

  log << "end cleaning\n";
  sp::on();
  threads::Threads::instance().cleaning_.release(
      static_cast<std::ptrdiff_t>(threads::Threads::instance().count()));

  log << "STW end, time: " << std::fixed << std::setprecision(3)
      << 1000 * static_cast<double>(clock() - s) / CLOCKS_PER_SEC << "ms \n";
  allocator_.dump();
}

fn GarbageCollector::tracing()->void {
  while (true) {
    for (int i = 0; i < stack_for_marked_.size(); ++i)
      log << "tracing: stack_for_marked[" << i << "] = " << stack_for_marked_[i]
          << "\n"; // log debug info

    if (let ptr = stack_for_marked_.pop(); ptr.has_value() && how_many_ref(ptr.value()))
      marking(ptr.value());
    else {
      log << "tracing: ohh, stack is empty(((\n";
      if (!threads::Threads::instance().count_of_working_threads_)
        throw std::runtime_error("fantom working thread");
      log << "with me, count_of_working_threads = "
          << threads::Threads::instance().count_of_working_threads_ << "\n";
      if (!--threads::Threads::instance().count_of_working_threads_)
        break;
      while (!stack_for_marked_.size())
        if (!threads::Threads::instance().count_of_working_threads_)
          return;
      ++threads::Threads::instance().count_of_working_threads_;
    }
  }
}

fn GarbageCollector::make_root_and_tracing(ref ssp)->void {
  log << "calling make_root_and_tracing\n";

  let hrtptr = threads::Threads::instance().get(ssp);
  log << hrtptr.start_sp << ":\n.\n.\n.\n"
      << ssp << "\n"
      << "summary diff: " /*<< std::hex*/
      << hrtptr.start_sp - ssp << "\n";

  assert(ssp < hrtptr.start_sp);
  for (ref sp = ssp; sp <= hrtptr.start_sp; sp += 8) {
    auto ptr = *reinterpret_cast<ref*>(sp);
    log << "make_root_and_tracing: on stack: " << ptr << "\n";
    if (ref_in_heap(ptr)) {
      log << "find " << ptr << " in root\n";
      // memory_.fetch_sub(how_many_ref(ptr));
      stack_for_marked_.push(ptr);
    }
  }
  log << "thread completed root\n";

  tracing();

  log << "thread completed tracing\n";

  threads::Threads::instance()
      .tracing_.release(); // tell gc we`re done! (for start cleaning)

  log << "thread end\n";
}