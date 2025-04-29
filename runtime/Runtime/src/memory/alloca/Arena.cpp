#include "Arena.hpp"

Arena::Arena(size_t arena_size, ref arena_start, size_t arena_tier)
    : size(arena_size), start(arena_start), tier(arena_tier), cur(arena_start),
      died(true), marked_(arena_start, arena_start + arena_size) {
}

fn Arena::revive()->void {
  guard(mutex_);
  died = false;
}

fn Arena::is_died() const->bool {
  return died;
}

[[nodiscard]] fn Arena::key_for_heap() const->size_t {
  return start + size - cur;
}

[[nodiscard]] fn Arena::uniq_for_heap() const->ref {
  return start;
}

fn Arena::is_empty() const->bool {
  return start == cur;
}