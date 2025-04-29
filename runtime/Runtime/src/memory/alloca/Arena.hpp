#pragma once
#include "BitMap.hpp"

#include <atomic>
#include <cassert>
#include <cstddef>
#include <mutex>
#include <tbb/concurrent_vector.h>
#include <utils/defines.h>

class Arena {
  bool died;

public:
  BitMap     marked_;
  std::mutex mutex_;

  tbb::concurrent_vector<size_t> objects;

  const size_t size;
  const ref    start;
  const size_t tier;

  ref cur;

  Arena(size_t arena_size, ref arena_start, size_t arena_tier);

  auto revive()->void;

  auto is_died() const->bool;

  [[nodiscard]] auto key_for_heap() const->size_t;
  [[nodiscard]] auto uniq_for_heap() const->ref;

  auto is_empty() const->bool;

  // 4 debug
  auto kill() {
    objects.clear();
  }
};
