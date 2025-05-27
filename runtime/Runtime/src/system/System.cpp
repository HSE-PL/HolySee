#include "System.hpp"

#include "utils/defines.h"

#include "Runtime.hpp" // dso
#include <cstdint>
#include <memory/MemoryManager.hpp>
#include <stddef.h>
#include <stdexcept>
#include <sys/mman.h>

auto sys::aalloc(Arena* arena) -> void {
  if (!arena)
    throw std::runtime_error("all regs are full");
  MemoryManager::memory += arena->size;

  auto p = mmap(reinterpret_cast<void*>(arena->start), arena->size,
                PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED,
                -1, 0);
#define return_arena MemoryManager::regions()[arena->tier]->push(arena)

  if (p == MAP_FAILED) {
    return_arena;
    throw std::runtime_error("bad_alloc: size of heap is too much");
  }

  if (p != reinterpret_cast<void*>(arena->start)) {
    return_arena;
    throw std::runtime_error("somethink went wrong");
  }
}

auto sys::free(Arena* a) -> void {
  // std::cout << "sizeof " << a->tier << ": "
  //           << MemoryManager::regions()[a->tier]->slots_.size() << "\\"
  //           << MemoryManager::regions()[a->tier]->size /
  //                  MemoryManager::regions()[a->tier]->t_size
  //           << std::endl;
  MemoryManager::regions()[a->tier]->push(a);

  // guard(MemoryManager::mutex_);
  MemoryManager::memory -= a->size;

  if (munmap(reinterpret_cast<void*>(a->start), a->size))
    throw std::runtime_error("2 free\n");
  a->cur = a->start;
}

auto sys::reserve(size_t size) -> void {
  auto p = mmap(nullptr, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (p == MAP_FAILED)
    throw std::runtime_error("heap is too much");
  MemoryManager::heap_start = reinterpret_cast<ref>(p);
}
