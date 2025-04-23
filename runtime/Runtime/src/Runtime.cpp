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
      log << info->si_addr << " " << sp::spd << "\n";
      if (info->si_addr != sp::spd)
        _exit(228);
      log << "ok\n";

      threads::Threads::instance().wait_end_sp();
      log << "handler: call make_root_and_tracing\n";

      ++threads::Threads::instance().count_of_working_threads_;
      gc->make_root_and_tracing(info, static_cast<ucontext_t*>(context));

      log << "handler: waiting ping from cleaning\n";
      threads::Threads::instance().cleaning_.acquire(); // waiting for the end of cleaning
    }

    fn init()->void {
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

  [[noreturn]] fn run()->void {
    log << "runtime is runing\n";
    for (uint64_t i = 0;; ++i) {
      std::this_thread::sleep_for(std::chrono::seconds(2));
      log << "sp off\n";
      sp::off();
    }
  }

  [[noreturn]] fn init(void (&__start)(), void** spdptr, void* sp)->void {
    log << "main stack: " << sp << "\n";
    signals::init();
    sp::init(spdptr);

    let heap_size  = min_heap;
    let heap_start = sys::salloc(heap_size);
    log << "heap_size: " << heap_size << "\n";

    gc.emplace(reinterpret_cast<size_t>(heap_start), heap_size);

    if (!gc.has_value())
      throw std::runtime_error("gc bobo");

    log << "start __start: " << reinterpret_cast<size_t>(__start) << "\n";

    threads::Threads::instance().count_of_working_threads_.store(0);
    threads::Threads::instance().append(__start);

    std::thread auto_gc(run);

    while (true) {
      if (sp::sp)
        gc->GC();
    }
  }
} // namespace rt

extern "C" void __rt_init(void (&__start)(), void** spdptr, void* sp) {
  rt::init(__start, spdptr, sp);
}

// literally malloc, but holy alloc
extern "C" void* __halloc(instance* inst) {
  log << "__halloc: alloca " << inst->name << ", size:" << inst->size << "\n";
  if (rt::gc.has_value()) {
    let ptr_on_object = reinterpret_cast<instance**>(rt::gc->alloc(inst));
    *ptr_on_object    = inst;
    return reinterpret_cast<void*>(reinterpret_cast<size_t>(ptr_on_object) + 8);
  }
  return nullptr;
}