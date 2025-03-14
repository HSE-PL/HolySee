#include "Threads.hpp"

namespace threads {
  Threads& Threads::instance() {
    static Threads thrds;
    return thrds;
  }
} // namespace threads

void threads::Threads::append(void (&func)()) {
  _mutex.lock();
  log << "try to append thrd\n";

  auto* thread = new std::thread(func);

  auto pthread = thread->native_handle();

  pthread_attr_t attr;
  void*          stack_addr;
  size_t         stack_size;
  pthread_getattr_np(pthread, &attr);
  pthread_attr_getstack(&attr, &stack_addr,
                        &stack_size);

  std::cout << "Thread "
            << " stack starts at: " << stack_addr
            << ", size: " << stack_size
            << " bytes\n";
  auto hrptr = Horoutine{
      thread, reinterpret_cast<size_t>(
                  stack_addr + stack_size -
                  4592)}; // hehehe
  log << std::hex << hrptr.start_sp << "\n";
  // log << hrptr << "\n";
  pool.insert(hrptr);
  log << "thrd append\n";
  log << "qwe---:" << std::hex
      << pool.begin()->start_sp << "\n";
  _mutex.unlock();
}

Horoutine threads::Threads::get(size_t sp) {
  log << "try to get mutex in get\n";
  _mutex.lock();
  // auto el = pool.lower_bound(sp);
  auto el = pool.begin();
  assert(el != pool.end());
  _mutex.unlock();
  return *el;
}
