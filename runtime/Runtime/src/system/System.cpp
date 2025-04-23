#include "System.hpp"

#include "utils/defines.h"

#include <cstdint>
#include <stddef.h>
#include <stdexcept>
#include <sys/mman.h>

fn sys::salloc(size_t size)->void* {
  let p = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  if (p == MAP_FAILED)
    throw std::bad_alloc();
  return p;
}
