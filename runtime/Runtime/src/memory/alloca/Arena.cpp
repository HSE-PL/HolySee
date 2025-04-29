#include "Arena.hpp"

Arena::Arena(size_t arena_size, ref arena_start, size_t arena_tier)
    : size(arena_size), start(arena_start), tier(arena_tier), cur(arena_start),
      died(true), marked_(arena_start, arena_start + arena_size, 8) {
}

auto Arena::revive() -> void {
  guard(mutex_);
  died = false;
}

auto Arena::is_died() const -> bool {
  return died;
}

[[nodiscard]] auto Arena::key_for_heap() const -> size_t {
  return start + size - cur;
}

[[nodiscard]] auto Arena::uniq_for_heap() const -> ref {
  return start;
}

auto Arena::is_empty() const -> bool {
  return start == cur;
}