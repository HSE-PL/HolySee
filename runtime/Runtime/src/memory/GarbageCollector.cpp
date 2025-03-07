#include "GarbageCollector.hpp"
#include "safepoints/Safepoint.hpp"

GarbageCollector::GarbageCollector(
    size_t start_heap, size_t size_heap)
    : Allocator(start_heap, size_heap),
      memory(size_heap) {
}

size_t
GarbageCollector::alloc(size_t size_object) {
  if (memory < 3 * (size >> 2))
    GC();

  memory -= size_object;
  return Allocator::alloc(size_object);
}

void GarbageCollector::GC() {
  sp::off();
}

void GarbageCollector::cleaning(
    siginfo_t* info, ucontext_t* context) {
  // TODO implement this shit
}
