#include <csignal>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <memory/GarbageCollector.hpp>
#include <ostream>
#include <pthread.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <sys/ucontext.h>
#include <thread>
#include <unistd.h>


namespace rt {

  namespace sp {
    void*  spdptr;
    size_t pagesize = 1 << 12;

    inline void change(int prot) {
      mprotect(spdptr, pagesize, prot);
    }

    inline void off() {
      change(PROT_NONE);
    }

    inline void on() {
      change(PROT_READ);
    }

    inline void init() {
      spdptr = mmap(NULL, pagesize, PROT_READ,
                    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    }
  }; // namespace sp

  namespace signals {

    void handler(int sig, siginfo_t* info, void* context) {
      if (info->si_addr != sp::spdptr)
        _exit(1);
    }

    void init() {
      struct sigaction sa;
      sa.sa_sigaction = handler;
      sigemptyset(&sa.sa_mask);
      sa.sa_flags = SA_SIGINFO;

      if (sigaction(SIGSEGV, &sa, NULL) == -1) {
        perror("unlucky, sigaction return -1");
        exit(1);
      }
    }
  }; // namespace signals

  GarbageCollector gc;

  inline void init(void* __start, void** spdptr) {
    signals::init();
    sp::spdptr = spdptr;
    sp::init();
    gc = GarbageCollector();
  }

}; // namespace rt

extern "C" void __rt_init(void* __start, void** spdptr) {
  rt::init(__start, spdptr);
}
