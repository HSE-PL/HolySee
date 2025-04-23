#pragma once
#include "ThreadSafeVector.hpp"
#include "alloca/Allocator.hpp"

#include <Runtime.hpp>
#include <csignal>
#include <stddef.h>
#include <sys/ucontext.h>
#include <threads/Threads.hpp>

class GarbageCollector {

  size_t memory_;
  BitMap checked_;

  Allocator allocator_;

  ThreadSafeVector<ref> stack_for_marked_;

  static size_t how_many_ref(instance* p) {
    return p->have_ref ? p->size : 0;
  }

  static size_t how_many_ref(ref p) {
    return how_many_ref(*reinterpret_cast<instance**>(p - 8));
  }

  fn tracing()->void;

  fn cleaning() {
    for (const auto& regs : allocator_.regions_) {
      regs->have_empty = false;
    }
    auto trash = std::vector<Arena*>();
    auto emty  = std::vector<Arena*>();
    log << "all regs without empty arena\n";

    for (const auto& arena : allocator_.heap_.keys) {
      log << "try check\n";
      log << "check " << arena->uniq_for_heap() << "\n";
      if (arena->is_died()) {
        log << "died\n";
        if (allocator_.regions_[arena->tier]->have_empty) {
          log << "died died\n";
          trash.push_back(arena);
          log << "free\n";
        } else {
          log << "just chill, this arena is empty\n";
          allocator_.regions_[arena->tier]->have_empty = true;
          emty.push_back(arena);
        }
      }
    }

    for (const auto& a : trash)
      allocator_.free_arena(a);

    for (const auto& a : emty)
      allocator_.revive(a);

    log << "end cleaning\n";
    // throw std::runtime_error("bebebe");
    // TODO implement this shit
  }

  void marking(ref ptr) {
    log << "marking: try to marking " << ptr << " type ("
        << (*reinterpret_cast<instance**>(ptr - 8))->name << ")\n";
    let arena = allocator_.arena_by_ptr(ptr);

    // allocator_.regions_[arena->tier]->mutex_.lock();
    // guard(allocator_.regions_[arena->tier]->mutex_);
    // log << "marking: mutex yep\n";
    if (allocator_.marking_.check_and_set(ptr)) {
      log << "unluck\n";
      return;
    }
    log << "marking: " << arena->uniq_for_heap() << " revive\n";
    arena->revive();
    log << "marking object: start\n";

    let size = how_many_ref(ptr);
    for (let offset = 8; offset <= size; offset += 8)
      marking_push(ptr + offset);
    log << "marking object: end\n";
  }

  void marking_push(ref ptr) {
    log << "marking_push: " << ptr << "\n";
    if (let field = *reinterpret_cast<ref*>(ptr); ref_in_heap(field))
      stack_for_marked_.push(field);
  }

  bool ref_in_heap(ref ptr) const {
    log << "ref_in_heap: " << ptr << "\n";
    return (allocator_.start_ <= ptr) && (ptr < allocator_.start_ + allocator_.size_);
  }

public:
  GarbageCollector(ref start_heap, size_t size_heap);

  void GC();
  void make_root_and_tracing(siginfo_t* info, ucontext_t* context);

  ref alloc(instance*);
};
