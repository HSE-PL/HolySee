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

ref GarbageCollector::alloc(size_t size_object) {
  log << memory_ << "\\" << size << "\n";
  if (memory_ < 3 * (size >> 2))
    GC();

  memory_ -= size_object;
  return Allocator::alloc(size_object); //<------
}

void GarbageCollector::GC() {
  log << "calling GC\n";
  print();
  sp::off();

  for (int i = 0; i < threads::Threads::instance().count(); ++i)
    root_was_claim_.acquire();

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
    std::cout << ptr << "\n";
    auto [index, clear_ptr] = split(ptr);
    if (start_ <= clear_ptr && clear_ptr < start_ + size_) {
      if (auto [size, have_ref] = how_many_ref(index); have_ref) {
        log << "find " << clear_ptr << " in root, type: " << index << "\n";
        root_.push({index, clear_ptr});
      }
    }
  }

  threads::Threads::instance().waitEndRooting();
  root_was_claim_.release();
}