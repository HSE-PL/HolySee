#include "Bitset.hpp"
#include <algorithm>
#include <cassert>
#include <utils/defines.h>
#include <vector>


void Bitset::set(size_t n) {
  bits[n / 64] |= n % 64;
}

uint64_t Bitset::get(size_t n) const {
  return bits[n / 64] & (1ULL << (n % 64));
}

void Bitset::clear(size_t from, size_t to) {
  assert(from < to);
  bits[from / 64] &= ~get_mask(from);
  bits[to / 64] &= get_mask(to);
  if (to / 64 - from / 64 > 1)
    std::fill(from / 64, to / 64, 0);
}

void Bitset::clear() {
  std::fill(bits.begin(), bits.end(), 0);
}
