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

  std::pair<size_t, bool> how_many_ref(uint64_t ptr) const {
    auto index = ptr >> 48;
    if (!index)
      return std::pair(0x8, false);
    if (index == 1)
      throw "index 1 is not supported";
    if (index < TT_RESERVED) {
      if (index & 1)
        return std::pair(1 << (index >> 1), true);
      return std::pair(1 << (index >> 1), false);
    }
    assert(index - TT_RESERVED >= tt_->size);
    return std::pair(tt_->md[index - TT_RESERVED], true);
  }

  std::pair<uint64_t, uint64_t> split(size_t ptr) const {
    return std::pair(ptr >> 48, clear(ptr));
  }

  void cleaning() {
    log << "calling cleaning\n";

    for (const auto ptr : root_) {
      marking(ptr);
    }

    for (const auto& regs : regions_) {
      regs->have_empty = false;
    }

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
    log << "try to marking " << fat_ptr.ptr << "type (" << fat_ptr.index << ")\n";
    auto arena = arena_by_ptr(fat_ptr.ptr);
    regions_[arena->tier]->mutex_.lock();
    if (marking_[fat_ptr.ptr]) {
      log << "unluck\n";
      regions_[arena->tier]->mutex_.unlock();
      return;
    }
    marking_.set(fat_ptr.ptr);
    arena->revive();

    for (long offset = 0; offset < tt_->md[fat_ptr.index]; offset += 8) {
      auto ptr_on_field = fat_ptr.ptr + offset;
      log << "find ptr on field: " << ptr_on_field << "\n";
      auto field = *reinterpret_cast<size_t*>(ptr_on_field);
      log << "value of field " << field << "\n";
      auto [field_ptr, field_type] = split(field);
      if (auto [size, have_ref] = how_many_ref(field_type); have_ref) {
        marking({field_ptr, field_type});
      }
    }
  }


public:
  inline static size_t TT_RESERVED = 128;
  GarbageCollector(size_t start_heap, size_t size_heap, TypeTable* tt);

  void GC();
  void make_root(siginfo_t* info, ucontext_t* context);

  ref alloc(size_t) override;
};
