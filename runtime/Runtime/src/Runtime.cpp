#include "Runtime.hpp"
#include "memory/GarbageCollector.hpp"
#include "safepoints/Safepoint.hpp"
#include "system/System.hpp"

#include <csignal>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <functional>
#include <iostream>
#include <optional>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/ucontext.h>
#include <thread>
#include <threads/Threads.hpp>
#include <unistd.h>

namespace rt {

  std::optional<GarbageCollector> gc;
  std::optional<threads::Threads> thrds;

  namespace signals {

    void handler(int sig, siginfo_t* info,
                 void* context) {
      // for (;;)
      // ;
      std::cout << info->si_addr << " " << sp::spd
                << std::endl;
      if (info->si_addr != sp::spd &&
          gc.has_value())
        _exit(228);
      gc.value().cleaning(
          info,
          static_cast<ucontext_t*>(context));
      // TODO implement this shit
    }

    void init() {
      struct sigaction sa {};
      sa.sa_sigaction = handler;
      sigemptyset(&sa.sa_mask);
      sa.sa_flags = SA_SIGINFO;

      if (sigaction(SIGSEGV, &sa, NULL) == -1) {
        perror("unlucky, sigaction return -1");
        exit(1);
      }
    }
  } // namespace signals


  inline void init(void (&__start)(),
                   void** spdptr) {
    signals::init();
    sp::init(spdptr);

    size_t heap_size = (1 << 5) * sp::pagesize;

    void* heap_start = sys::salloc(heap_size);

    gc.emplace(GarbageCollector(
        reinterpret_cast<size_t>(heap_start),
        heap_size));
    if (!gc.has_value())
      throw std::runtime_error("gc bobo");
    thrds.emplace(threads::Threads());

    std::cout << "start __start: " << __start
              << std::endl;

    // adition a thread in which sigfault
    thrds->append(__start);

    // __halloc(300);
    for (;;)
      ;
  }

} // namespace rt

extern "C" void __rt_init(void (&__start)(),
                          void** spdptr) {
  rt::init(__start, spdptr);
}

extern "C" void* __halloc(size_t size) {
  return rt::gc.has_value()
             ? reinterpret_cast<void*>(
                   rt::gc->alloc(
                       size)) //<--------
             : nullptr;
}

extern "C" void __GC() {
  rt::gc.value().GC();
}
