#include "Runtime.hpp"
#include "memory/GarbageCollector.hpp"
#include "safepoints/Safepoint.hpp"
#include "system/System.hpp"

#include "utils/log.h"
#include <assert.h>
#include <csignal>
#include <cstddef>
#include <cstdio>
#include <cstring>
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

  namespace signals {

    void handler(int sig, siginfo_t* info, void* context) {
      // for (;;)
      // ;
      log << info->si_addr << " " << sp::spd << "\n";
      if (info->si_addr != sp::spd && gc.has_value())
        _exit(228);
      log << "ok\n";

      threads::Threads::instance().waitEndSp();

      gc->make_root(info, static_cast<ucontext_t*>(context));
      threads::Threads::instance().marking_.acquire();
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

  [[noreturn]] void run() {
    for (long i = 0;; ++i) {
      // std::binary_semaphore sem(0);
      std::this_thread::sleep_for(std::chrono::seconds(3));
      threads::Threads::instance().rooting_.acquire();
      gc->GC();
    }
  }

  [[noreturn]] void init(void (&__start)(), void** spdptr, void* sp, TypeTable* tt) {
    log << "main stack: " << sp << "\n";
    signals::init();
    sp::init(spdptr);

    size_t heap_size = (1 << 5) * sp::pagesize;

    void* heap_start = sys::salloc(heap_size);

    gc.emplace(reinterpret_cast<size_t>(heap_start), heap_size, tt);
    if (!gc.has_value())
      throw std::runtime_error("gc bobo");

    log << "start __start: " << reinterpret_cast<size_t>(__start) << "\n";
    threads::Threads::instance().append(__start);

    run();
  }
} // namespace rt

extern "C" void __rt_init(void (&__start)(), void** spdptr, void* sp, TypeTable* tt) {
  rt::init(__start, spdptr, sp, tt);
}

extern "C" void* __halloc(size_t size) {
  std::cout << "alloca " << size << "\n";
  assert(size);
  return rt::gc.has_value() ? reinterpret_cast<void*>(rt::gc->alloc(size)) : nullptr;
}

extern "C" void __GC() {
  rt::gc.value().GC();
}