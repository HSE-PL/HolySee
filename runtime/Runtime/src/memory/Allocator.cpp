#include "Allocator.hpp"

#include "MemoryManager.hpp"
#include "utils/defines.h"
#include "utils/log.h"

#include "Runtime.hpp"
#include <safepoints/Safepoint.hpp>
#include <sstream>
#include <system/System.hpp>

Allocator::Allocator() {
}

Allocator& Allocator::instance() {
  static Allocator hallocator;
  return hallocator;
}

auto Allocator::alloc(size_t object_size) -> ref {
  guard(mutex_);
  auto arena = heap.get_min_more_then(object_size);
  if (!arena ||
      arena->start + arena->size - arena->cur > ((object_size >> 12) << 12) + 1_page)
    return add_active(object_size)->start;

  heap.del(arena);
  arena->cur += object_size;
  heap.append(arena);


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
  auto tier0 = 0;
  for (; ((1 << tier0) + 1) * 1_page < object_size; ++tier0)
    ;
  for (auto tier = tier0; tier < MemoryManager::COUNT_OF_TIERS; ++tier) {
    auto new_active_size = ((1 << tier0) + 1) * 1_page;

    auto start = sys::salloc(new_active_size, tier0);

    if (start) {
      auto new_active = new Arena(new_active_size, reinterpret_cast<ref>(start), tier0);
      heap.append(new_active);
      logezhe << "return from add_active\n";

      return new_active;
    }
  }
  throw false;
}

auto Allocator::free_arena(Arena* a) -> void {
  guard(mutex_);
  a->kill(); // debug
  // logezhe << "?\n";
  heap.del(a);
  MemoryManager::free_arena(a);
}

// auto Allocator::revive(Arena* a)->void {
//   free_arena(a);
//   add_active(a->tier);
//   // heap.append(a);
//   logezhe << "revive: end\n";
// }

// 4 debug and test only
auto Allocator::free(ref ptr) -> void {
  guard(mutex_);
  logezhe << "free     " << ptr << "\n";
  logezhe << "on arena " << MemoryManager::arena_by_ptr(ptr)->start << "\n";
  auto abp = MemoryManager::arena_by_ptr(ptr);
  // emplaced_.clear(abp->start, abp->start + abp->size);
  free_arena(abp);
}

auto Allocator::dump() const -> void {
  return;
  // std::ostringstream logezhe;
  // logezhe << std::hex;
  //
  // for (const auto& r : regions_) {
  //   logezhe << r->t_size << " region : {";
  //   for (const auto& arena : heap.keys) {
  //     if (arena->size == r->t_size) {
  //       logezhe << "arena " << arena->start << " (";
  //       // guard(arena->mutex_);
  //       for (const auto& obj_size : arena->objects) {
  //         logezhe << "[" << obj_size << "],";
  //       }
  //       logezhe << "... " << arena->key_for_heap() << "), <"
  //           << regions_[arena->tier]->size_pull() << "> ";
  //     }
  //   }
  //   logezhe << "}\n";
  // }

  // logezhe << oss.str();
}
