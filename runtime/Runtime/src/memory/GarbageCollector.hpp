#pragma once
#include "ThreadSafeVector.hpp"
#include "alloca/Allocator.hpp"

#include <Runtime.hpp>
#include <csignal>
#include <stddef.h>
#include <sys/ucontext.h>
#include <threads/Threads.hpp>

class GarbageCollector final : public Allocator {

  size_t     memory_;
  BitMap     checked_;
  TypeTable* tt_;


  struct fatPtr {
    uint64_t index;
    uint64_t ptr;
  };
  ThreadSafeVector<fatPtr> root_;

  std::counting_semaphore<> root_was_claim_{0};

  size_t clear(size_t ptr) const {
    return ptr << 16 >> 16;
  }

  std::pair<size_t, bool> how_many_ref(size_t index) const {
    if (!index)
      return std::pair(0x8, false);
    if (index == 1)
      throw "index 1 is not supported";
    if (index < TT_RESERVED)
      return std::pair(1 << (index >> 1), index & 1);
    if (129 < index && index < 134)
      return std::pair(tt_->md[index - TT_RESERVED], false);
    assert(index - TT_RESERVED < tt_->size);
    log << "true\n";
    return std::pair(tt_->md[index - TT_RESERVED], true);
  }

  std::pair<uint64_t, uint64_t> split(size_t ptr) const {
    return std::pair(ptr >> 48, clear(ptr));
  }

  void cleaning() {
    log << "calling cleaning rootlen: " << root_.size() << "\n";

    for (const auto ptr : root_) {
      log << "marking ptr " << ptr.ptr << "\n";
      marking(ptr);
      // for (;;)
      // ;
    }
    log << "GC: marking end\n";

    for (const auto& regs : regions_) {
      regs->have_empty = false;
    }
    log << "all regs without empty arena\n";
    for (const auto arena : keys) {
      if (arena->is_died()) {
        if (regions_[arena->tier]->have_empty) {
          keys.erase(arena);
        } else {
          regions_[arena->tier]->have_empty = true;
        }
      }
    }
    // throw std::runtime_error("bebebe");
    // TODO implement this shit
  }

  void marking(const fatPtr& fat_ptr) {
    log << "try to marking " << fat_ptr.ptr << " type (" << fat_ptr.index << ")\n";
    auto arena = arena_by_ptr(fat_ptr.ptr);
    guard(regions_[arena->tier]->mutex_);
    if (marking_[fat_ptr.ptr]) {
      log << "unluck\n";
      // regions_[arena->tier]->mutex_.unlock();
      return;
    }
    marking_.set(fat_ptr.ptr);
    log << "marking: " << arena->uniq_for_heap() << " revive\n";
    arena->revive();
    if (how_many_ref(fat_ptr.index).second) {
      for (long offset = 0; offset < tt_->md[fat_ptr.index - TT_RESERVED]; offset += 8) {
        // for (;;)
        //   ;
        auto ptr_on_field = fat_ptr.ptr + offset;
        log << "find ptr on field: " << ptr_on_field << "\n";
        auto field = *reinterpret_cast<size_t*>(ptr_on_field);
        log << "value of field " << field << "\n";
        if (ref_in_heap(field)) {
          auto [field_type, field_ptr] = split(field);
          log << "find " << field_ptr << " in field of " << fat_ptr.ptr << "(" << fat_ptr.index
              << "), type: " << field_type << "\n";
          marking({field_type, field_ptr});
        }
      }
    }
  }

  bool ref_in_heap(ref ptr) const {
    auto clear_ptr = clear(ptr);
    // log << clear_ptr << "\n";
    return (start_ <= clear_ptr) && (clear_ptr < start_ + size_);
  }

public:
  inline static size_t TT_RESERVED = 128;
  GarbageCollector(size_t start_heap, size_t size_heap, TypeTable* tt);

  void GC();
  void make_root(siginfo_t* info, ucontext_t* context);

  ref alloc(size_t) override;
};
