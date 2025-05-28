#include "BitMap.hpp"

auto BitMap::set(ref n) -> void {
  guard(mutex_);
  bitset_[map(n)] = true;
}

auto BitMap::unset(ref n) -> void {
  guard(mutex_);
  bitset_[map(n)] = false;
}

auto BitMap::clear(uint64_t start, size_t count) -> void {
  guard(mutex_);
  std::fill_n(bitset_.begin() + start, count, false);
}


auto BitMap::clear() -> void {
  guard(mutex_);
  bitset_.clear();
}

auto BitMap::check_and_set(ref n) -> bool {
  guard(mutex_);
  auto check = bitset_[map(n)];
  set(n);
  return check;
}

auto BitMap::operator[](ref n) const -> bool {
  guard(mutex_);
  return bitset_[map(n)];
}