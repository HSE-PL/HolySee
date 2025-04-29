#include "BitMap.hpp"

auto BitMap::set(ref n) -> void {
  guard(mutex_);
  bitset_.set(map(n));
}

auto BitMap::unset(ref n) -> void {
  guard(mutex_);
  bitset_.reset(map(n));
}


// void BitMap::clear() {
//   guard(mutex_);
//   Bitset::clear();
// }

auto BitMap::clear() -> void {
  guard(mutex_);
  bitset_.clear();
}

auto BitMap::check_and_set(ref n) -> bool {
  guard(mutex_);
  auto mask = bitset_[map(n)];
  set(n);
  return mask;
}

auto BitMap::operator[](ref n) const -> bool {
  guard(mutex_);
  return bitset_[map(n)];
}