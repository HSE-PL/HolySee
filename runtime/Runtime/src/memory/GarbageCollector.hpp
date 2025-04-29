#pragma once
#include "Allocator.hpp"
#include "MemoryManager.hpp"
#include "threads/Threads.hpp"
#include <Runtime.hpp>
#include <csignal>
#include <stddef.h>
#include <sys/ucontext.h>
#include <tbb/concurrent_queue.h>
#include <threads/Threads.hpp>

class GarbageCollector {

  tbb::concurrent_queue<ref> queue_for_marked_;

  inteval<instance> inteval_;

  static size_t how_many_ref(instance* p) {
    return p->have_ref ? p->size : 0;
  }

  size_t how_many_ref(ref p) {
    auto i = *reinterpret_cast<instance**>(p - 8);
    if (inteval_.correct(i))
      return how_many_ref(i);
    return 0;
  }

  auto tracing() -> void;

  auto cleaning() {
    logezhe << "start cleaning\n";
    for (const auto& arena : Allocator::instance().heap.keys) {
      // log << "try check\n";
      // log << "check " << arena->uniq_for_heap() << "\n";
      if (arena->is_died()) {
        Allocator::instance().free_arena(arena);
      }
    }
    // logezhe << "after cleaning: len0 = " << allocator_.regions_[0]->size_pull() <<
    // "\n"; logezhe << "after cleaning: len1 = " << allocator_.regions_[1]->size_pull()
    // << "\n"; logezhe << "after cleaning: len2 = " <<
    // allocator_.regions_[2]->size_pull() << "\n"; throw std::runtime_error("bebebe");
  }

  auto marking(ref ptr) {
    // logezhe << "marking: try to marking " << ptr << " type ("
    //     << (*reinterpret_cast<instance**>(ptr - 8))->name << ")\n";
    auto arena = MemoryManager::arena_by_ptr(ptr);
    // guard(allocator_.regions_[arena->tier]->mutex_);
    // logezhe << "marking: mutex yep\n";
    if (arena->marked_.check_and_set(ptr)) {
      logezhe << "unluck\n";
      return;
    }
    // logezhe << "marking: " << arena->uniq_for_heap() << " revive\n";
    arena->revive();
    // logezhe << "marking object: start\n";

    auto size = how_many_ref(ptr);
    for (auto offset = 0; offset < size; offset += 8)
      marking_push(ptr + offset);
    // logezhe << "marking object: end\n";
  }

  void marking_push(ref ptr) {
    if (auto field = *reinterpret_cast<ref*>(ptr); ref_in_heap(field)) {
      logezhe << "marking_push: " << ptr << "\n";
      queue_for_marked_.push(field);
    }
  }

  auto ref_in_heap(ref ptr) const -> bool {
    logezhe << "ref_in_heap: " << ptr << "\n";
    return MemoryManager::ref_in_reg(ptr);
  }

public:
  GarbageCollector(instance*, instance*);

  void GC();
  void make_root_and_tracing(ref ssp);
};
