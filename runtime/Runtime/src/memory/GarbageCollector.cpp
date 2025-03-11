#include "GarbageCollector.hpp"
#include "safepoints/Safepoint.hpp"

GarbageCollector::GarbageCollector(
    size_t start_heap, size_t size_heap)
    : Allocator(start_heap, size_heap),
      memory(size_heap) {
}

size_t
GarbageCollector::alloc(size_t size_object) {

  std::cout << memory << "\\" << size
            << std::endl;
  if (memory < 3 * (size >> 2))
    GC();

  memory -= size_object;
  return Allocator::alloc(size_object); //<------
}

void GarbageCollector::GC() {
  std::cout << "calling GC\n";
  sp::off();
}

void GarbageCollector::cleaning(
    siginfo_t* info, ucontext_t* context) {
  size_t ssp =
      context->uc_mcontext.gregs[REG_RSP];
  for (int i = 0; i < 10; ++i) {
    std::cout << *reinterpret_cast<size_t*>(ssp +
                                            8 * i)
              << std::endl;
  }
  throw std::runtime_error("bebebe");
  // TODO implement this shit
}
