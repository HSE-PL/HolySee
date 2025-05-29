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

auto threads::Threads::append(void (&func)(...), const va_list args) -> void {
  guard(mutex_);

  std::binary_semaphore s(0);
  std::binary_semaphore e(0);

  const auto thread_is_died = std::make_shared<bool>(false);

  const auto thread = std::make_shared<std::thread>([&]() {
    s.acquire();
    std::cout << "start\n";
    e.release();
    func(args);
    std::cout << "end\n";
    *thread_is_died = true;
  });

  const auto pthread = thread->native_handle();

  pthread_attr_t attr;

  void*  stack_addr;
  size_t stack_size;
  pthread_getattr_np(pthread, &attr);
  pthread_attr_getstack(&attr, &stack_addr, &stack_size);
  pthread_attr_destroy(&attr);

  auto hrptr = Horoutine{
      thread, reinterpret_cast<size_t>(stack_addr) + stack_size - 4592,
      thread_is_died}; // hehehe
  pool_.insert(hrptr);
  s.release();
  e.acquire();
  // std::cout << "thrd append\n";
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
