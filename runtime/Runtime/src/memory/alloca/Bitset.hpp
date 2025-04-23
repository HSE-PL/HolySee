#pragma once
#include "utils/defines.h"
#include <assert.h>
#include <cstdint>
#include <vector>
class Bitset {
  std::vector<uint64_t> bits;

  fn get_mask(size_t n)->uint64_t {
    assert(n < 64);

    return ((1ULL << (64 - (n % 64))) - 1ULL);
  }

public:
  Bitset(size_t count_bit) : bits((count_bit - 1) / 64 + 1, 0) {
  }

  fn set(size_t n)->void;

  fn get(size_t n) const->size_t;

  fn clear()->void;

  fn clear(size_t from, size_t to)->void;
};