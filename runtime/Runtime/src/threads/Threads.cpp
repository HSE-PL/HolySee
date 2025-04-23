#include "Threads.hpp"

namespace threads {
  Threads& Threads::instance() {
    static Threads thrds;
    return thrds;
  }
} // namespace threads

fn threads::Threads::append(void (&func)())->void {
  guard(mutex_);
  log << "try to append thrd\n";
  ++count_of_working_threads_;

  auto* thread = new std::thread(func);

  auto pthread = thread->native_handle();

  pthread_attr_t attr;

  void*  stack_addr;
  size_t stack_size;
  pthread_getattr_np(pthread, &attr);
  pthread_attr_getstack(&attr, &stack_addr, &stack_size);

  std::cout << "Thread "
            << " stack starts at: " << stack_addr << ", size: " << stack_size << " bytes\n";
  auto hrptr =
      Horoutine{thread, reinterpret_cast<size_t>(stack_addr) + stack_size - 4592}; // hehehe
  log << std::hex << hrptr.start_sp << "\n";
  pool_.insert(hrptr);
  log << "thrd append\n";
}

Horoutine threads::Threads::get(size_t sp) {
  guard(mutex_);
  auto el = pool_.lower_bound(sp);
  assert(el != pool_.end());
  return *el;
}

fn threads::Threads::wait_end_sp()->void {
  if (releaseIfAll(sp_))
    return;
  sp_.acquire();
}

fn threads::Threads::wait_end_tracing()->void {
  if (releaseIfAll(rooting_))
    return;
  rooting_.acquire();
}
