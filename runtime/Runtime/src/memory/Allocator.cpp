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
  logezhe << "alloc: at this moment arena: " << arena << "\n";
  if (arena)
    logezhe << "oh, this arena have size: " << arena->size << "\n";
  if (!arena || (arena->start + arena->size - arena->cur >
                 std::max((object_size >> 12) << 12, 1_page) + 1_page) &&
                    MemoryManager::regions()[arena->tier]->empty())
    return add_active(object_size)->start;

  heap.del(arena);
  arena->cur += object_size;
  heap.append(arena);
  arena->objects.push_back(object_size);


  // log << "huh\n";
  // arena->objects.push(object_size);
  // log << "huh\n";
  // dump();

  // dump();
  // log << "alloc: end\n";
  return arena->cur - object_size;
}

auto Allocator::add_active(size_t object_size) -> Arena* {
  guard(mutex_);
  logezhe << "call add active\n";
  auto tier = 0;
  for (; ((1 << tier) + 1) * 1_page < object_size; ++tier)
    ;

  auto new_active_size = MemoryManager::regions()[tier]->t_size;
  auto new_active      = sys::alloc(new_active_size, tier);

  new_active->cur += object_size;
  heap.append(new_active);
  new_active->objects.push_back(object_size);
  logezhe << "return from add_active\n";

  return new_active;
}

auto Allocator::free_arena(Arena* a) -> void {
  guard(mutex_);
  // a->kill();
  // logezhe << "?\n";
  heap.del(a);
  MemoryManager::free_arena(a);
  logezhe << "b\n";
}

// 4 debug and test only
auto Allocator::free(ref ptr) -> void {
  guard(mutex_);
  logezhe << "free     " << ptr << "\n";
  logezhe << "on arena " << MemoryManager::arena_by_ptr(ptr)->start << "\n";
  auto abp = MemoryManager::arena_by_ptr(ptr);
  // emplaced_.clear(abp->start, abp->start + abp->size);
  free_arena(abp);
}