#include "GarbageCollector.hpp"
#include "safepoints/Safepoint.hpp"
#include "threads/Threads.hpp"
#include "utils/log.h"
#include <cassert>

GarbageCollector::GarbageCollector(
    size_t start_heap, size_t size_heap)
    : Allocator(start_heap, size_heap),
      memory(size_heap) {
}

size_t
GarbageCollector::alloc(size_t size_object) {

  log << memory << "\\" << size << "\n";
  if (memory < 3 * (size >> 2))
    GC();

  memory -= size_object;
  return Allocator::alloc(size_object); //<------
}

void GarbageCollector::GC() {
  log << "calling GC\n";
  sp::off();
}

void GarbageCollector::cleaning(
    siginfo_t* info, ucontext_t* context) {
  log << "calling cleaning\n";
  size_t ssp =
      context->uc_mcontext.gregs[REG_RSP];

  auto hrtptr =
      threads::Threads::instance().get(ssp);
  log << hrtptr.start_sp << ":\n.\n.\n.\n"
      << ssp << "\n"
      << "summary diff: " /*<< std::hex*/
      << hrtptr.start_sp - ssp << "\n";

  assert(ssp < hrtptr.start_sp);
  for (size_t sp = ssp; sp <= hrtptr.start_sp;
       sp += 8) {
    std::cout << *reinterpret_cast<size_t*>(sp)
              << "\n";
  }
  throw std::runtime_error("bebebe");
  // TODO implement this shit
}
