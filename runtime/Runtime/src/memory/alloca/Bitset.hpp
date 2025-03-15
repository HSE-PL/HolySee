#pragma once
#include <assert.h>
#include <cstdint>
#include <vector>

class Bitset {
  std::vector<uint64_t> bits;

  uint64_t get_mask(size_t n) {
    assert(n < 64);

    return ((1ULL << (64 - (n % 64))) - 1ULL);
  }

public:
  Bitset(size_t count_bit) : bits((count_bit - 1) / 64 + 1, 0) {
  }

  void set(size_t n);

  size_t get(size_t n) const;

  void clear();

  void clear(size_t from, size_t to);
};