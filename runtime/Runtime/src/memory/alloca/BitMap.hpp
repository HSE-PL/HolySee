#pragma once
#include "utils/defines.h"
#include "utils/log.h"
#include <boost/dynamic_bitset.hpp>

#include <cassert>
#include <mutex>

class BitMap {

  mutable std::recursive_mutex mutex_;

  boost::dynamic_bitset<> bitset_;

  auto map(size_t n) const -> size_t {
    assert(from_ <= n);
    assert(n < to_);
    return (n - from_) / k_;
  }

public:
  ref    from_;
  ref    to_;
  size_t k_;
  BitMap(ref from, ref to, size_t k)
      : k_(k), bitset_((to - from) / k), from_(from), to_(to) {
  }

  auto set(ref n) -> void;

  auto unset(ref n) -> void;

  auto operator[](ref n) const -> bool;

  auto check_and_set(ref n) -> bool;

  auto clear() -> void;

  auto clear(uint64_t start, size_t count) -> void;
};
