#include "System.hpp"

#include "utils/defines.h"

#include "Runtime.hpp" // dso
#include <cstdint>
#include <memory/MemoryManager.hpp>
#include <stddef.h>
#include <stdexcept>
#include <sys/mman.h>

auto sys::salloc(size_t size, size_t tier) -> void* {
  guard(MemoryManager::mutex_);
  MemoryManager::memory += size;

  logezhe << "salloc: " << size << "\n";

  void* start_new_object = nullptr;

  if (MemoryManager::regions_[tier].slots_.empty()) {
    if (MemoryManager::cur + size > Allocator::instance().heap_start + rt::max_heap_size)
      return nullptr;

    start_new_object = MemoryManager::cur;
    MemoryManager::cur += MemoryManager::regions_[tier].t_size;
  } else {
    if (!MemoryManager::regions_[tier].slots_.try_pop(start_new_object))
      return nullptr;
  }

  auto p = mmap(start_new_object, size, PROT_READ | PROT_WRITE,
                MAP_SHARED | MAP_ANONYMOUS, -1, 0);

  if (p == MAP_FAILED)
    throw std::runtime_error("bad_alloc: size of");

  MemoryManager::cur += size;
  return p;
}

auto sys::sfree(Arena* a) -> void {
  MemoryManager::regions_[a->tier].slots_.push(reinterpret_cast<void*>(a->start));

  guard(MemoryManager::mutex_);
  MemoryManager::memory -= a->size;

  if (munmap(reinterpret_cast<void*>(a->start), a->size))
    throw std::runtime_error("2 free\n");
}

auto sys::sreserve(size_t size) -> void {
  auto p = mmap(nullptr, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (p == MAP_FAILED)
    throw std::runtime_error("heap is too much");
  MemoryManager::cur = p;
}
