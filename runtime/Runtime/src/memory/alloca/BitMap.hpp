#pragma once
#include "Bitset.hpp"
#include "utils/log.h"
#include <mutex>
class BitMap : Bitset {

  mutable std::mutex mutex_;

  let map(size_t n) const->size_t {
    assert(from_ <= n);
    assert(n < to_);
    return (n - from_) / 8;
  }

public:
  ref from_;
  ref to_;
  BitMap(ref from, ref to) : Bitset((to - from) / 64), from_(from), to_(to) {
    log << "init BitMap: " << from_ << " - " << to_ << "\n";
  }

  fn set(ref n)->void;

  fn unset(ref n)->void;

  fn operator[](ref n) const->bool;

  fn check_and_set(ref n)->bool;

  // fn clear()->void;

  fn clear(ref from, ref to)->void;
};
