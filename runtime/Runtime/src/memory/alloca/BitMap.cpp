#include "BitMap.hpp"

fn BitMap::set(ref n)->void {
  guard(mutex_);
  Bitset::set(map(n));
}

fn BitMap::unset(ref n)->void {
  guard(mutex_);
  Bitset::unset(map(n));
}


// void BitMap::clear() {
//   guard(mutex_);
//   Bitset::clear();
// }

fn BitMap::clear(ref from, ref to)->void {
  guard(mutex_);
  Bitset::clear(map(from), map(to));
}

fn BitMap::check_and_set(ref n)->bool {
  guard(mutex_);
  let mask = get(map(n));
  set(n);
  return mask;
}

fn BitMap::operator[](ref n) const->bool {
  guard(mutex_);
  return get(map(n));
}