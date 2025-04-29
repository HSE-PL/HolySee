#pragma once
#include "Allocator.hpp"
#include "alloca/Arena.hpp"
#include "bitset"
#include "utils/defines.h"
#include <array>
#include <safepoints/Safepoint.hpp>
#include <unordered_map>
struct Region {
  size_t count;
  size_t t_size;
};

class MemoryManager {
  let static constexpr COUNT_OF_TIERS = 48;
  std::unordered_map<ref, Arena*> static inline ref_to_arena_{};

public:
  std::mutex static inline mutex_;
  std::atomic_size_t static inline memory = 0;
  let static inline regions_              = [] {
    std::array<Region, COUNT_OF_TIERS> regs{};
    for (let i = 0; i < COUNT_OF_TIERS; ++i) {
      regs[i].count  = 0;
      regs[i].t_size = ((1 << i) + 1) * 1_page;
    }
    return regs;
  }();

  let static active_pages_()
      ->std::atomic<std::bitset<1ULL << (sizeof(size_t) * 8 - 16 - 12)>>& {
    static std::atomic<std::bitset<1ULL << (sizeof(size_t) * 8 - 16 - 12)>> bm;
    return bm;
  }

  let static ref_in_reg(ref ptr)->bool {
    log << "ref_in_reg: from " << active_pages_().from_ << " - " << active_pages_().to_
        << "\n";
    return active_pages_()[ptr >> 12];
  }

  let static arena_by_ptr(ref ptr)->Arena* {
    return ref_to_arena_[ptr];
  }

  let static free_arena(Arena* a)->void {
    guard(mutex_);
    ref_to_arena_.erase(a->start);
    active_pages_().unset(a->start >> 12);
    regions_[a->tier].count--;
  }
};
