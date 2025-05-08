#include "Allocator.hpp"

#include "MemoryManager.hpp"
#include "utils/defines.h"
#include "utils/log.h"

#include "Runtime.hpp"
#include <safepoints/Safepoint.hpp>
#include <sstream>
#include <system/System.hpp>

Allocator::Allocator() = default;

Allocator& Allocator::instance() {
  static Allocator hallocator;
  return hallocator;
}

auto Allocator::alloc(size_t object_size) -> ref {
  guard(mutex_);
  auto arena = heap.get_min_more_then(object_size);
  if (!arena ||
      (arena->start + arena->size - arena->cur >
           std::max((object_size >> 12) << 12, 1_page) + 1_page &&
       !MemoryManager::regions()[MemoryManager::tier_by_size(object_size)]
            ->empty())) [[unlikely]]
    return add_active(object_size)->start;

  heap.del(arena);
  arena->cur += object_size;
  heap.append(arena);

  arena->mutex_.lock();
  arena->objects.push_back(object_size);
  arena->mutex_.unlock();

  return arena->cur - object_size;
}

auto Allocator::add_active(size_t object_size) -> Arena* {
  guard(mutex_);
  auto&& regs = MemoryManager::regions();
  Arena* arena;
  for (auto tier = 0; tier < regs.size(); ++tier) {
    // std::cout << regs[tier]->slots_.size() << std::endl;
    if (((1 << tier) + 1) * 1_page >= object_size) {
      if (auto a = regs[tier]->pop(); a.has_value()) {
        arena = a.value();
        sys::aalloc(arena);

        arena->cur += object_size;
        heap.append(arena);

        arena->mutex_.lock();
        arena->objects.push_back(object_size);
        arena->mutex_.unlock();


        return arena;
      }
    }
  }
  throw std::runtime_error("no available arenas");
}

auto Allocator::free_arena(Arena* a) -> void {
  guard(mutex_);
  heap.del(a);
  MemoryManager::free_arena(a);
}

// 4 debug and test only
auto Allocator::free(ref ptr) -> void {
  guard(mutex_);
  auto abp = MemoryManager::arena_by_ptr(ptr);
  free_arena(abp);
}