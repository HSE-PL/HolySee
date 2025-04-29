#pragma once
#include "BitMap.hpp"
#include <cassert>
#include <cstddef>
#include <memory/ThreadSafeVector.hpp>
#include <mutex>
#include <utils/defines.h>

class Arena {
  bool died;

public:
  BitMap     marked_;
  std::mutex mutex_;

  ThreadSafeVector<size_t> objects;

  const size_t size;
  const ref    start;
  const size_t tier;

  ref cur;

  Arena(size_t arena_size, ref arena_start, size_t arena_tier);

  fn revive()->void;

  fn is_died() const->bool;

  [[nodiscard]] fn key_for_heap() const->size_t;
  [[nodiscard]] fn uniq_for_heap() const->ref;

  fn is_empty() const->bool;

  // 4 debug
  fn kill() {
    objects.clear();
  }
};
