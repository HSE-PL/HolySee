#include "System.hpp"

#include "utils/defines.h"

#include "Runtime.hpp" // dso
#include <cstdint>
#include <memory/MemoryManager.hpp>
#include <stddef.h>
#include <stdexcept>
#include <sys/mman.h>

auto sys::alloc(size_t size, size_t tier) -> Arena* {
  guard(MemoryManager::mutex_);
  auto&& regs = MemoryManager::regions();

  logezhe << "salloc: " << size << "\n";

  Arena* arena = nullptr;

  for (auto i = tier; i < regs.size(); ++i) {
    if (!regs[i]->empty()) {
      arena = regs[i]->pop();
      break;
    }
  }
  if (!arena)
    throw std::runtime_error("all regs are full");
  MemoryManager::memory += arena->size;

  // std::cout << "start:" << arena->start << std::endl;

  auto p = mmap(reinterpret_cast<void*>(arena->start), arena->size,
                PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED,
                -1, 0);

  if (p == MAP_FAILED)
    throw std::runtime_error("bad_alloc: size of heap is too much");

  if (p != reinterpret_cast<void*>(arena->start))
    throw std::runtime_error("somethink went wrong");

  return arena;
}

auto sys::free(Arena* a) -> void {
  MemoryManager::regions()[a->tier]->push(a);

  // guard(MemoryManager::mutex_);
  MemoryManager::memory -= a->size;

  if (munmap(reinterpret_cast<void*>(a->start), a->size))
    throw std::runtime_error("2 free\n");
}

auto sys::reserve(size_t size) -> void {
  auto p = mmap(nullptr, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (p == MAP_FAILED)
    throw std::runtime_error("heap is too much");
  MemoryManager::heap_start = reinterpret_cast<ref>(p);
  // std::cout << MemoryManager::heap_start << ":" << size << std::endl;
}
