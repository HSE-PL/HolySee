#include "System.hpp"

#include <stddef.h>
#include <sys/mman.h>

void* sys::salloc(size_t size) {
  return mmap(NULL, size, PROT_READ | PROT_WRITE,
              MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
}