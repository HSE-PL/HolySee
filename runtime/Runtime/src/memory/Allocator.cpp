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

fn Allocator::alloc(size_t object_size)->ref {
  guard(mutex_);
  let arena = heap.get_min_more_then(object_size);
  if (!arena ||
      arena->start + arena->size - arena->cur > ((object_size >> 12) << 12) + 1_page)
    return add_active(object_size)->start;


  heap.del(arena);
  arena->cur += object_size;
  heap.append(arena);


  log << "huh\n";
  // arena->objects.push(object_size);
  log << "huh\n";
  // dump();

  // dump();
  log << "alloc: end\n";
  return arena->cur - object_size;
}

fn Allocator::add_active(size_t object_size)->Arena* {
  guard(mutex_);
  log << "call add active\n";
  let tier = 0;
  for (; ((1 << tier) + 1) * 1_page < object_size; ++tier)
    ;
  let new_active_size = ((1 << tier) + 1) * 1_page;
  let start           = sys::salloc(new_active_size);

  let new_active = new Arena(new_active_size, reinterpret_cast<ref>(start), tier);
  heap.append(new_active);
  log << "return from add_active\n";
  return new_active;
}

fn Allocator::free_arena(Arena* a)->void {
  guard(mutex_);
  a->kill(); // debug
  // log << "?\n";
  heap.del(a);
  MemoryManager::free_arena(a);
}

// fn Allocator::revive(Arena* a)->void {
//   free_arena(a);
//   add_active(a->tier);
//   // heap.append(a);
//   log << "revive: end\n";
// }

// 4 debug and test only
fn Allocator::free(ref ptr)->void {
  guard(mutex_);
  log << "free     " << ptr << "\n";
  log << "on arena " << MemoryManager::arena_by_ptr(ptr)->start << "\n";
  let abp = MemoryManager::arena_by_ptr(ptr);
  // emplaced_.clear(abp->start, abp->start + abp->size);
  free_arena(abp);
}

fn Allocator::dump() const->void {
  return;
  // std::ostringstream log;
  // log << std::hex;
  //
  // for (const auto& r : regions_) {
  //   log << r->t_size << " region : {";
  //   for (const auto& arena : heap.keys) {
  //     if (arena->size == r->t_size) {
  //       log << "arena " << arena->start << " (";
  //       // guard(arena->mutex_);
  //       for (const auto& obj_size : arena->objects) {
  //         log << "[" << obj_size << "],";
  //       }
  //       log << "... " << arena->key_for_heap() << "), <"
  //           << regions_[arena->tier]->size_pull() << "> ";
  //     }
  //   }
  //   log << "}\n";
  // }

  // log << oss.str();
}
