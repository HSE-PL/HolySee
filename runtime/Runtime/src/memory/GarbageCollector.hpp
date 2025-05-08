#pragma once
#include "Allocator.hpp"
#include "MemoryManager.hpp"
#include "threads/Threads.hpp"
#include <Runtime.hpp>
#include <csignal>
#include <stddef.h>
#include <sys/ucontext.h>
#include <threads/Threads.hpp>

class GarbageCollector {

  ThreadSafeVector<ref> stack_for_marked_;

  inteval<instance> inteval_;


  static size_t how_many_ref(instance* p) {
    return p->have_ref ? p->size : 0;
  }

  size_t how_many_ref(ref p) {
    auto i = *reinterpret_cast<instance**>(p - 1_ref);
    if (inteval_.correct(i))
      return how_many_ref(i);
    return 0;
  }

  auto tracing() -> void;

  static auto cleaning() {
    auto trash = std::vector<Arena*>();

    for (const auto& arena : Allocator::instance().heap.keys) {
      if (arena->is_died())
        trash.push_back(arena);
    }

    for (const auto& trash_arena : trash)
      Allocator::instance().free_arena(trash_arena);

    for (const auto& a : Allocator::instance().heap.keys)
      a->temp_kill();
  }

  auto marking(ref ptr) {
    if (ref_is_real(ptr)) {
      auto i = (*reinterpret_cast<instance**>(ptr - 1_ref));
      logezhe << "marking: try to marking " << ptr << " type (" << i->name
              << ")\n";
    }
    auto arena = MemoryManager::arena_by_ptr(ptr);
    if (arena->marked_.check_and_set(ptr))
      return;

    arena->revive();
    auto size = how_many_ref(ptr);
    for (auto offset = 0; offset < size; offset += 1_ref) {
      if (ref_is_real(ptr + offset)) {
        if (auto field = *reinterpret_cast<ref*>(ptr); ref_is_real(field)) {
          logezhe << "marking_push: " << ptr << "\n";
          stack_for_marked_.push(field);
        }
      }
    }
    logezhe << "marking object: end\n";
  }

  auto ref_is_real(ref ptr) -> bool {
    return MemoryManager::ref_in_heap(ptr) && how_many_ref(ptr);
  }

public:
  GarbageCollector(instance*, instance*);

  std::atomic_uint_fast64_t num_of_cleaning = 0;
  void                      GC();
  void                      make_root_and_tracing(ref ssp);
};
