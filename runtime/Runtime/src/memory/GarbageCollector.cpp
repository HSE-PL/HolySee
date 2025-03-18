#include "GarbageCollector.hpp"
#include "safepoints/Safepoint.hpp"
#include "threads/Threads.hpp"
#include "utils/log.h"
#include <cassert>
#include <cstddef>

GarbageCollector::GarbageCollector(size_t start_heap, size_t size_heap, TypeTable* tt)
    : Allocator(start_heap, size_heap), checked_(start_heap, start_heap + size_heap),
      memory_(size_heap), tt_(tt) {
}

ref GarbageCollector::alloc(size_t type) {
  log << memory_ << "\\" << size_ << "\n";
  if (memory_ < 3 * (size_ >> 2))
    sp::off();

  auto size_object = how_many_ref(type).first;
  memory_ -= size_object;
  return Allocator::alloc(size_object) | (type << 48);
}

void GarbageCollector::GC() {
  log << "calling GC\n";
  print();
  // sp::off();

  for (int i = 0; i < threads::Threads::instance().count(); ++i)
    root_was_claim_.acquire();
  log << "rooting end\n";
  cleaning();
  log << "STW end \n";
  print();
}

void GarbageCollector::make_root(siginfo_t* info, ucontext_t* context) {
  log << "calling make_root\n";
  ref ssp = context->uc_mcontext.gregs[REG_RSP];

  auto hrtptr = threads::Threads::instance().get(ssp);
  log << hrtptr.start_sp << ":\n.\n.\n.\n"
      << ssp << "\n"
      << "summary diff: " /*<< std::hex*/
      << hrtptr.start_sp - ssp << "\n";

  assert(ssp < hrtptr.start_sp);
  for (ref sp = ssp; sp <= hrtptr.start_sp; sp += 8) {
    auto ptr = *reinterpret_cast<ref*>(sp);
    log << "make_root: on stack: " << ptr << "\n";
    if (ref_in_heap(ptr)) {
      auto [index, clear_ptr] = split(ptr);
      log << "find " << clear_ptr << " in root, type: " << index << "\n";
      root_.push({index, clear_ptr});
    }
  }

  threads::Threads::instance().waitEndRooting();
  root_was_claim_.release();
}