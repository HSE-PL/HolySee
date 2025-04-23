#include "GarbageCollector.hpp"
#include "safepoints/Safepoint.hpp"
#include "threads/Threads.hpp"
#include "utils/log.h"
#include <cassert>
#include <cstddef>

GarbageCollector::GarbageCollector(ref start_heap, size_t size_heap)
    : checked_(start_heap, start_heap + size_heap), memory_(size_heap),
      allocator_(start_heap, size_heap) {
}

fn GarbageCollector::alloc(instance* inst)->ref {
  log << memory_ << "\\" << allocator_.size_ << "\n";
  if (memory_ < 7 * (allocator_.size_ >> 3))
    sp::off();

  memory_ -= inst->size;
  return allocator_.alloc(inst->size + 8);
}

fn GarbageCollector::GC()->void {
  log << "calling GC\n";
  allocator_.heap_.print();
  for (int i = 0; i < threads::Threads::instance().count(); --i)
    threads::Threads::instance().tracing_was_complete_.acquire();
  log << "rooting end\n";

  cleaning();
  log << "STW end \n";
  allocator_.heap_.print();
}

fn GarbageCollector::tracing()->void {
  while (true) {
    for (int i = 0; i < stack_for_marked_.size(); ++i)
      log << "tracing: stack_for_marked[" << i << "] = " << stack_for_marked_[i]
          << "\n"; // log debug info

    if (let ptr = stack_for_marked_.pop(); ptr.has_value())
      marking(ptr.value());
    else {
      if (!threads::Threads::instance().count_of_working_threads_)
        throw std::runtime_error("fantom working thread");
      if (!--threads::Threads::instance().count_of_working_threads_)
        break;
      while (!stack_for_marked_.size())
        if (!threads::Threads::instance().count_of_working_threads_)
          return;
      ++threads::Threads::instance().count_of_working_threads_;
    }
  }
}

fn GarbageCollector::make_root_and_tracing(siginfo_t* info, ucontext_t* context)->void {
  log << "calling make_root_and_tracing\n";
  const ref ssp = context->uc_mcontext.gregs[REG_RSP];

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
      stack_for_marked_.push(ptr);
    }
  }
  log << "thread completed root\n";

  tracing();

  threads::Threads::instance().tracing_was_complete_.release();
  threads::Threads::instance().wait_end_tracing();
}