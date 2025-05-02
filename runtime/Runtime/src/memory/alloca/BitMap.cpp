#include "BitMap.hpp"

auto BitMap::set(ref n) -> void {
  guard(mutex_);
  bitset_.set(map(n));
}

auto BitMap::unset(ref n) -> void {
  guard(mutex_);
  bitset_.reset(map(n));
}

auto BitMap::clear(uint64_t start, size_t count) -> void {
  bitset_.reset(map(start), count);
}


auto BitMap::clear() -> void {
  guard(mutex_);
  bitset_.reset();
}

auto BitMap::check_and_set(ref n) -> bool {
  guard(mutex_);
  auto check = bitset_.test(map(n));
  set(n);
  return check;
}

auto BitMap::operator[](ref n) const -> bool {
  guard(mutex_);
  return bitset_[map(n)];
}