#pragma once
#include <cassert>
#include <cstddef>
#include <mutex>
#include <utils/defines.h>
class Arena {
  bool died;

  std::mutex mutex_;

public:
  const size_t size;
  const ref    start;
  const size_t tier;

  size_t cur;

  Arena(size_t arena_size, ref arena_start, size_t arena_tier);

  fn revive()->void;

  fn is_died() const->bool;

  [[nodiscard]] fn key_for_heap() const->size_t;
  [[nodiscard]] fn uniq_for_heap() const->ref;

  fn is_empty() const->bool;
};
