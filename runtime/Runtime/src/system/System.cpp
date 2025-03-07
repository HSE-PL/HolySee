#include "System.hpp"

#include <stddef.h>
#include <sys/mman.h>

namespace sys {
  void* salloc(size_t size) {
    return mmap(
        nullptr, size, PROT_READ | PROT_WRITE,
        MAP_ANONYMOUS | MAP_ANONYMOUS, -1, NULL);
  }
} // namespace sys