#pragma once
#include <cstddef>

class Arena {
public:
  const size_t size;
  const size_t start;
  const size_t tier;

  size_t cur;

  Arena(const size_t arena_size, const size_t arena_start,
        const size_t arena_tier)
      : size(arena_size), start(arena_start), tier(arena_tier),
        cur(arena_start) {}

  [[nodiscard]] size_t key_for_heap() const { return start + size - cur; }

  [[nodiscard]] size_t uniq_for_heap() const { return start; }
};