#include "Threads.hpp"

namespace threads {
  Threads& Threads::instance() {
    static Threads thrds;
    return thrds;
  }
} // namespace threads

auto threads::Threads::deappend(ref sp) -> void {
  pool_.extract(pool_.lower_bound(sp));
}


auto threads::Threads::get(size_t sp) -> Horoutine {
  guard(mutex_);
  auto el = pool_.lower_bound(sp);
  assert(el != pool_.end());
  return *el;
}

auto threads::Threads::wait_end_sp() -> void {
  releaseIfAll(sp_);
  sp_.acquire();
}
