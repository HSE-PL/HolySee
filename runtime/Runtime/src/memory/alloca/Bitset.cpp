#include "Bitset.hpp"
#include <algorithm>
#include <cassert>
#include <utils/defines.h>
#include <vector>


fn Bitset::set(size_t n)->void {
  bits[n / 64] |= n % 64;
}

fn Bitset::get(size_t n) const->uint64_t {
  return bits[n / 64] & (1ULL << (n % 64));
}

fn Bitset::clear(size_t from, size_t to)->void {
  assert(from < to);
  bits[from / 64] &= ~get_mask(from);
  bits[to / 64] &= get_mask(to);
  if (to / 64 - from / 64 > 1)
    std::fill(&bits[from / 64], &bits[to / 64], 0);
}

fn Bitset::clear()->void {
  std::fill(bits.begin(), bits.end(), 0);
}
