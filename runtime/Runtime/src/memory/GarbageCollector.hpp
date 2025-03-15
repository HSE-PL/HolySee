#pragma once
#include "ThreadSafeVector.hpp"
#include "alloca/Allocator.hpp"

#include <Runtime.hpp>
#include <csignal>
#include <stddef.h>
#include <stl_stack.h>
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
    return ptr & ((1 << 48) - 1);
  }

  std::pair<size_t, bool> how_many_ref(size_t ptr) const {
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
    return tt_->md[index - TT_RESERVED], true;
  }

  std::pair<size_t, size_t> split(size_t ptr) const {
    return std::pair(ptr >> 48, clear(ptr));
  }

  void cleaning() {
    log << "calling cleaning\n";


    // throw std::runtime_error("bebebe");
    // TODO implement this shit
  }

  void marking(fatPtr fat_ptr) {
    auto arena = arena_by_ptr(fat_ptr.ptr);
    if (marking_[fat_ptr.ptr]) {
      return;
    }
    marking_.set(fat_ptr.ptr);

    regions_[arena->tier].mutex_.lock();
  }


public:
  inline static size_t TT_RESERVED = 128;
  GarbageCollector(size_t start_heap, size_t size_heap, TypeTable* tt);

  void GC();
  void make_root(siginfo_t* info, ucontext_t* context);

  ref alloc(size_t) override;
};
