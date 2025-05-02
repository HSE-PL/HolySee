#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Runtime.hpp"
#include "memory/Allocator.hpp"
#include "memory/GarbageCollector.hpp"
#include "memory/MemoryManager.hpp"
#include "safepoints/Safepoint.hpp"
#include "system/System.hpp"
#include "threads/Threads.hpp"
#include "utils/log.h"
#include "utils/stb_image_write.h"
#include <SFML/Graphics.hpp>
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
#include <unistd.h>
namespace rt {

  std::optional<GarbageCollector> gc;

  auto gogc(ref ssp) -> void {
    threads::Threads::instance().wait_end_sp();
    logezhe << "handler: call make_root_and_tracing\n";

    ++threads::Threads::instance().count_of_working_threads_;
    gc->make_root_and_tracing(ssp);

    logezhe << "handler: waiting ping from cleaning\n";
    threads::Threads::instance()
        .cleaning_.acquire(); // waiting for the end of cleaning
  }

  auto aom() -> void {
    void* a;
    sp::off();
    gogc(reinterpret_cast<ref>(&a));
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

  void draw_rectangle(uint8_t* img, int img_width, int x, int y, int w, int h,
                      uint8_t r, uint8_t g, uint8_t b, int border) {
    assert(w > 2 * border);
    assert(h > 2 * border);
    for (int i = y; i < y + h; ++i) {
      for (int j = x; j < x + w; ++j) {
        int idx = (i * img_width + j) * 3;
        if ((i < border + y) || (j < border + x) || (i > y + h - border) ||
            (j > x + w - border)) {
          img[idx + 0] = 0;
          img[idx + 1] = 0;
          img[idx + 2] = 0;
        } else {
          img[idx + 0] = r;
          img[idx + 1] = g;
          img[idx + 2] = b;
        }
      }
    }
  }

  void draw() {
    static int c = 0;
    c++;
    const int w = 2100;
    const int h = 100;

    std::vector<uint8_t> img(w * h * 3);
    std::memset(img.data(), 255, img.size());
    const int offset = 5;
    const int pfb    = 1;

    auto regw = w - 2 * offset;

    size_t regsize = 0;
    for (const auto& reg : MemoryManager::regions())
      regsize = std::max(regsize, reg->size);

    auto sizepp = regsize / regw;

    auto regh = (h - offset) / MemoryManager::regions().size();
    regh -= offset;
    assert(regh);

    for (auto regnum = 0; regnum < MemoryManager::regions().size(); regnum++) {
      auto curreg = MemoryManager::regions()[regnum];

      auto regfloor = offset + regnum * (regh + offset);

      for (auto arenanum = 0; arenanum < curreg->items_.size(); arenanum++) {
        auto curarena = (*curreg)[arenanum];

        auto arenaw = curarena->size / sizepp;

        auto countobj = curarena->objects.size();
        if (countobj) {
          draw_rectangle(img.data(), w, offset + arenaw * arenanum, regfloor,
                         arenaw, regh, 142, 128, 156, pfb);
        } else {
          draw_rectangle(img.data(), w, offset + arenaw * arenanum, regfloor,
                         arenaw, regh, 255, 42, 42, pfb);
        }

        auto offset_obj = 0;
        // std::cout << curarena->objects.size() << std::endl;
        for (auto num_object = 0; num_object < countobj; num_object++) {
          auto objw = curarena->objects[num_object] / sizepp;
          assert(objw > 2 * pfb);
          draw_rectangle(img.data(), w, offset + arenaw * arenanum + offset_obj,
                         regfloor, objw, regh, 33, 242, 60, pfb);
          offset_obj += objw;
        }
      }
    }
    std::string name = "screens/heap";
    name += std::to_string(c);
    std::cout << name << std::endl;
    stbi_write_png((name + std::string(".png")).c_str(), w, h, 3, img.data(),
                   w * 3);
  }


  [[noreturn]] auto init(void (&__start)(), void** spdptr, void* sp,
                         instance* meta, instance* end, size_t max_heap_size)
      -> void {
    MemoryManager::max_heap_size =
        max_heap_size ? max_heap_size : MemoryManager::max_heap_size;
    sys::reserve(MemoryManager::max_heap_size);

    logezhe << "main stack: " << sp << "\n";
    signals::init();
    sp::init(spdptr);

    MemoryManager::COUNT_OF_TIERS = MemoryManager::regions().size();

    gc.emplace(meta, end);

    if (!gc.has_value())
      throw std::runtime_error("gc bobo");

    logezhe << "start __start: " << reinterpret_cast<size_t>(__start) << "\n";

    threads::Threads::instance().count_of_working_threads_.store(0);
    threads::Threads::instance().append(__start);


    while (true) {
      if (sp::sp)
        gc->GC();
    }
  }

} // namespace rt


extern "C" void __rt_init(void (&__start)(), void** spdptr, void* sp,
                          instance* meta, instance* end, size_t max_size_heap) {
  rt::init(__start, spdptr, sp, meta, end, max_size_heap);
}


// literally malloc, but sfree alloc
extern "C" void* __halloc(instance* inst) {
  logezhe << "__halloc: alloca " << inst->name << ", size:" << inst->size
          << "\n";
  static auto c = 0;
  c++;
  if (c == 174)
    std::cout << "";
  if (MemoryManager::max_heap_size && (MemoryManager::memory + inst->size >
                                       MemoryManager::max_heap_size >> 1)) {
    std::cout << c << ") AOM!\n";
    rt::aom();
  }

  ref ptr = 0;
  while (!ptr) {
    try {
      ptr = Allocator::instance().alloc(inst->size + 8);
    } catch (std::exception& e) {
      std::cout << "extra aom!\n";
      rt::aom();
    }
  }
  rt::draw();
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
  logezhe
      << "thread "
      << threads::Threads::instance().get(reinterpret_cast<size_t>(&a)).start_sp
      << " sleep " << n << "\n";
  std::this_thread::sleep_for(std::chrono::seconds(n));
}
