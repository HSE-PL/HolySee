#include "System.hpp"

#include <stddef.h>
#include <sys/mman.h>

void* sys::salloc(size_t size) {
  return mmap((void*)(1 << 48), size, PROT_READ | PROT_WRITE,
              MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
}