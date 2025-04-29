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
#include <memory/Allocator.hpp>
#include <memory/MemoryManager.hpp>
#include <optional>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/ucontext.h>
#include <thread>
#include <threads/Threads.hpp>
#include <unistd.h>
namespace rt {

  std::optional<GarbageCollector> gc;

  auto gogc(ref ssp) -> void {
    threads::Threads::instance().wait_end_sp();
    logezhe << "handler: call make_root_and_tracing\n";

    ++threads::Threads::instance().count_of_working_threads_;
    gc->make_root_and_tracing(ssp);

    logezhe << "handler: waiting ping from cleaning\n";
    threads::Threads::instance().cleaning_.acquire(); // waiting for the end of cleaning
  }

  namespace signals {

    auto handler(int sig, siginfo_t* info, void* context) {
      logezhe << info->si_addr << " (sp: " << sp::spd << ")\n";
      if (info->si_addr != sp::spd)
        _exit(228);
      // for (;;) {
      // logezhe << "@";
      // }
      logezhe << "ok\n";

      gogc(static_cast<ucontext_t*>(context)->uc_mcontext.gregs[REG_RSP]);
    }


    auto init() -> void {
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

  [[noreturn]] auto run() -> void {
    logezhe << "runtime is runing\n";
    for (auto i = 0;; ++i) {
      std::this_thread::sleep_for(std::chrono::seconds(2));
      logezhe << "sp off\n";
      sp::off();
      // for (;;)
      // ;
    }
  }

  [[noreturn]] auto init(void (&__start)(), void** spdptr, void* sp, instance* meta,
                         instance* end, size_t max_heap_size) -> void {
    rt::max_heap_size = max_heap_size ? max_heap_size : rt::max_heap_size;

    logezhe << "main stack: " << sp << "\n";
    signals::init();
    sp::init(spdptr);


    gc.emplace(meta, end);

    if (!gc.has_value())
      throw std::runtime_error("gc bobo");

    logezhe << "start __start: " << reinterpret_cast<size_t>(__start) << "\n";

    threads::Threads::instance().count_of_working_threads_.store(0);
    threads::Threads::instance().append(__start);

    // std::thread auto_gc(run);

    while (true) {
      if (sp::sp)
        gc->GC();
    }
  }

} // namespace rt


extern "C" void __rt_init(void (&__start)(), void** spdptr, void* sp, instance* meta,
                          instance* end, size_t max_size_heap) {
  rt::init(__start, spdptr, sp, meta, end, max_size_heap);
}

// literally malloc, but sfree alloc
extern "C" void* __halloc(instance* inst) {
  logezhe << "__halloc: alloca " << inst->name << ", size:" << inst->size << "\n";
  if (rt::max_heap_size) {
    if (MemoryManager::memory + inst->size > rt::max_heap_size) {
      void* a;
      sp::off();
      rt::gogc(reinterpret_cast<ref>(&a));
    }
  }
  auto ptr = Allocator::instance().alloc(inst->size + 8);
  if (!ptr)
    throw std::runtime_error("AOM!");

  auto ptr_on_object = reinterpret_cast<instance**>(ptr);
  logezhe << ptr_on_object << "\n";
  *ptr_on_object = inst;
  return reinterpret_cast<void*>(reinterpret_cast<size_t>(ptr_on_object) + 8);
}

extern "C" void __go(void (&func)()) {
  threads::Threads::instance().append(func);
}

extern "C" void __sleep(size_t n) {
  int a;
  logezhe << "thread "
          << threads::Threads::instance().get(reinterpret_cast<size_t>(&a)).start_sp
          << " sleep " << n << "\n";
  std::this_thread::sleep_for(std::chrono::seconds(n));
}
