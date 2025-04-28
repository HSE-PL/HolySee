#pragma once
#include "ThreadSafeVector.hpp"
#include "alloca/Allocator.hpp"
#include "threads/Threads.hpp"

#include <Runtime.hpp>
#include <csignal>
#include <stddef.h>
#include <sys/ucontext.h>
#include <threads/Threads.hpp>

class GarbageCollector {

  BitMap checked_;

  Allocator allocator_;

  ThreadSafeVector<ref> stack_for_marked_;

  inteval<instance> inteval_;

  static size_t how_many_ref(instance* p) {
    return p->have_ref ? p->size : 0;
  }

  size_t how_many_ref(ref p) {
    let i = *reinterpret_cast<instance**>(p - 8);
    if (inteval_.correct(i))
      return how_many_ref(i);
    return 0;
  }

  fn tracing()->void;

  fn cleaning() {
    log << "start cleaning\n";
    for (const auto& regs : allocator_.regions_) {
      regs->have_empty = false;
    }
    let trash = std::vector<Arena*>();
    let emty  = std::vector<Arena*>();
    // log << "all regs without empty arena\n";

    for (const auto& arena : allocator_.heap_.keys) {
      // log << "try check\n";
      // log << "check " << arena->uniq_for_heap() << "\n";
      if (arena->is_died()) {
        // log << "died\n";
        if (allocator_.regions_[arena->tier]->have_empty) {
          // log << "died died\n";
          trash.push_back(arena);
          // log << "free\n";
        } else {
          // log << "just chill, this arena is empty\n";
          allocator_.regions_[arena->tier]->have_empty = true;
          emty.push_back(arena);
        }
      }
    }
    log << "cleaning: trash " << trash.size() << " and empty " << emty.size()
        << " collect\n";

    for (auto& a : trash) {
      log << "!\n";

      allocator_.free_arena(a);
    }
    log << "pupu\n";

    for (auto& a : emty)
      allocator_.revive(a);
    // for (;;)
    //   ;

    // log << "after cleaning: len0 = " << allocator_.regions_[0]->size_pull() << "\n";
    // log << "after cleaning: len1 = " << allocator_.regions_[1]->size_pull() << "\n";
    // log << "after cleaning: len2 = " << allocator_.regions_[2]->size_pull() << "\n";
    // throw std::runtime_error("bebebe");
    // TODO implement this shit
  }

  void marking(ref ptr) {
    // log << "marking: try to marking " << ptr << " type ("
    //     << (*reinterpret_cast<instance**>(ptr - 8))->name << ")\n";
    let arena = allocator_.arena_by_ptr(ptr);
    // guard(allocator_.regions_[arena->tier]->mutex_);
    // log << "marking: mutex yep\n";
    if (allocator_.marking_.check_and_set(ptr)) {
      log << "unluck\n";
      return;
    }
    // log << "marking: " << arena->uniq_for_heap() << " revive\n";
    arena->revive();
    // log << "marking object: start\n";

    let size = how_many_ref(ptr);
    for (let offset = 0; offset < size; offset += 8)
      marking_push(ptr + offset);
    // log << "marking object: end\n";
  }

  void marking_push(ref ptr) {
    if (let field = *reinterpret_cast<ref*>(ptr); ref_in_heap(field)) {
      log << "marking_push: " << ptr << "\n";
      stack_for_marked_.push(field);
    }
  }

  bool ref_in_heap(ref ptr) const {
    // log << "ref_in_heap: " << ptr << "\n";
    return (allocator_.start_ <= ptr) && (ptr < allocator_.start_ + allocator_.size_);
  }

public:
  GarbageCollector(ref start_heap, size_t size_heap, instance*, instance*);

  void GC();
  void make_root_and_tracing(ref ssp);

  ref alloc(instance*);
};
