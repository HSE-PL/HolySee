#pragma once
#include <cstddef>

class Arena {
public:
  const size_t size;
  const size_t start;

  size_t cur;

  Arena(const size_t arena_size,
        const size_t arena_start)
      : size(arena_size), start(arena_start),
        cur(arena_start) {}

  [[nodiscard]] size_t key_for_heap() const {
    return start + size - cur;
  }
};