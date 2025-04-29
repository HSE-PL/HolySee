#pragma once
#include "Allocator.hpp"
#include "alloca/Arena.hpp"
#include "utils/defines.h"
#include <array>
#include <oneapi/tbb/concurrent_queue.h>
#include <unordered_map>
struct Region {
  std::atomic<size_t>          count;
  size_t                       t_size;
  tbb::concurrent_queue<void*> slots_;

  Region(size_t c, size_t s) : count(c), t_size(s) {
  }
  Region& operator=(const Region& other) {
    count.store(other.count);
    t_size = other.t_size;
    slots_ = other.slots_;
    return *this;
  }

private:
  std::mutex mutex_;
};

class MemoryManager {
  std::unordered_map<ref, Arena*> static inline ref_to_arena_{};

  auto static active_pages_() -> BitMap& {
    auto static bm = BitMap(0, 1ULL << 47, 1_page);
    return bm;
  }

public:
  auto static constexpr COUNT_OF_TIERS = 48;
  static inline void* cur              = nullptr;

  std::mutex static inline mutex_;
  std::atomic_size_t static inline memory = 0;

  auto static inline regions_ = [] {
    std::array<Region, COUNT_OF_TIERS> regs{};
    for (auto i = 0; i < COUNT_OF_TIERS; ++i) {
      regs[i] = Region(0, ((1 << i) + 1) * 1_page);
    }
    return &regs;
  }();

  auto static ref_in_reg(ref ptr) -> bool {
    if (ptr < 1ULL << 47)
      return active_pages_()[ptr];
    return false;
  }

  auto static arena_by_ptr(ref ptr) -> Arena* {
    return ref_to_arena_[ptr];
  }

  auto static free_arena(Arena* a) -> void {
    guard(mutex_);
    ref_to_arena_.erase(a->start);
    active_pages_().set(a->start);
    regions_[a->tier].count--;
  }
};
