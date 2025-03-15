#pragma once
#include <cassert>
#include <cstddef>
#include <utils/defines.h>

class Arena {
  bool died;

public:
  const size_t size;
  const ref    start;
  const size_t tier;

  size_t cur;

  Arena(size_t arena_size, ref arena_start, size_t arena_tier)
      : size(arena_size), start(arena_start), tier(arena_tier), cur(arena_start), died(true) {
  }

  void revive() {
    assert(died);
    died = false;
  }

  [[nodiscard]] size_t key_for_heap() const {
    return start + size - cur;
  }

  [[nodiscard]] ref uniq_for_heap() const {
    return start;
  }
};
