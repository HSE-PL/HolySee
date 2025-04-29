#include "System.hpp"

#include "utils/defines.h"

#include "Runtime.hpp" // dso
#include <cstdint>
#include <memory/MemoryManager.hpp>
#include <stddef.h>
#include <stdexcept>
#include <sys/mman.h>

fn sys::salloc(size_t size)->void* {
  MemoryManager::memory += size;
  log << "salloc: " << size << "\n";
  let p = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  if (p == MAP_FAILED)
    throw std::runtime_error("bad_alloc: size of");
  return p;
}

fn sys::holy(Arena* a)->void {
  MemoryManager::memory -= a->size;
  if (munmap(reinterpret_cast<void*>(a->cur), a->size))
    throw std::runtime_error("2 free\n");
}
