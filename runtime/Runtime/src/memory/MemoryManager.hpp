#pragma once
#include "Allocator.hpp"
#include "alloca/Arena.hpp"
#include "alloca/Region.hpp"
#include "utils/defines.h"

#include <Runtime.hpp>
#include <array>
#include <oneapi/tbb/concurrent_queue.h>
#include <system/System.hpp>
#include <unordered_map>


namespace MemoryManager {
  extern std::recursive_mutex mutex_;

  auto partion_of_pages() -> std::vector<size_t>;

  extern size_t COUNT_OF_TIERS;

  extern size_t max_heap_size;
  extern ref    heap_start;

  extern std::atomic_size_t memory;


  auto regions() -> std::vector<Region<Arena>*>&;

  auto ref_in_heap(ref ptr) -> bool;

  auto arena_by_ptr(ref ptr) -> Arena*;

  auto free_arena(Arena* a) -> void;
}; // namespace MemoryManager
