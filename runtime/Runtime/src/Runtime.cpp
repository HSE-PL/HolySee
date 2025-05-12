#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Runtime.hpp"
#include "memory/Allocator.hpp"
#include "memory/GarbageCollector.hpp"
#include "memory/MemoryManager.hpp"
#include "safepoints/Safepoint.hpp"
#include "system/System.hpp"
#include "threads/Threads.hpp"
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

  static bool need_draw = false;

  std::optional<GarbageCollector> gc;

  auto handle_aom() -> void {
    void* a;
    sp::off();
    gc->gogc(reinterpret_cast<ref>(&a));
    draw();
  }

  namespace signals {

    auto handler(int sig, siginfo_t* info, void* context) {
      if (info->si_addr != sp::spd) {
        _exit(228);
      }
      gc->gogc(static_cast<ucontext_t*>(context)->uc_mcontext.gregs[REG_RSP]);
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
    for (auto i = 0;; ++i) {
      std::this_thread::sleep_for(std::chrono::seconds(2));
      sp::off();
    }
  }

  void draw_rectangle(uint8_t* img, int img_width, int x, int y, int w, int h,
                      uint8_t r, uint8_t g, uint8_t b, int border) {
    assert(w > 2 * border);
    assert(h > 2 * border);
    for (long long i = y; i < y + h; ++i) {
      for (long long j = x; j < x + w; ++j) {
        long long idx = (i * img_width + j) * 3;
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
    // return;
    static int      c = 1;
    const long long w = 2100;
    const long long h = 1000;

    std::vector<uint8_t> img(w * h * 3);
    std::memset(img.data(), 255, img.size());
    const int offset = 50;
    const int pfb    = 3;

    auto regw = w - 2 * offset;

    size_t regsize = 0;

    std::atomic_thread_fence(std::memory_order_acquire);
    for (const auto& reg : MemoryManager::regions())
      regsize = std::max(regsize, reg->size);

    auto sizepp = regsize / regw + 1;

    auto regh = (h - offset) / MemoryManager::regions().size();
    regh -= offset;
    assert(regh);

    for (auto regnum = 0; regnum < MemoryManager::regions().size(); regnum++) {
      auto curreg = MemoryManager::regions()[regnum];

      auto regfloor = offset + regnum * (regh + offset);

      for (auto arenanum = 0; arenanum < curreg->items_.size(); arenanum++) {
        auto curarena = (*curreg)[curreg->items_.size() - arenanum - 1];

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
    c++;
  }


  [[noreturn]] auto init(void (&__start)(), void** spdptr, void* sp,
                         instance* meta, instance* end, size_t max_heap_size)
      -> void {
    MemoryManager::max_heap_size =
        max_heap_size ? max_heap_size : MemoryManager::max_heap_size;
    sys::reserve(MemoryManager::max_heap_size);

    signals::init();
    sp::init(spdptr);

    MemoryManager::COUNT_OF_TIERS = MemoryManager::regions().size();

    gc.emplace(meta, end);

    if (!gc.has_value())
      throw std::runtime_error("gc bobo");

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


// literally malloc, but sfree aalloc
extern "C" void* __halloc(instance* inst) {
  static auto memory_limit = MemoryManager::max_heap_size >> 1;
  static auto c            = 0;
  // if (!(c & 0xfff))
  // std::cout << c << ") " << std::hex << MemoryManager::memory << std::endl;
  c++;
  if (MemoryManager::max_heap_size &&
      (MemoryManager::memory + inst->size > memory_limit)) [[unlikely]] {
    rt::draw();
    std::cout << c << ") AOM!\n";
    rt::handle_aom();
  }

  ref ptr = 0;
  for (auto i = 0; !ptr; ++i) {
    try {
      ptr = Allocator::instance().alloc(inst->size + 8);
    } catch (std::exception& e) {
      std::cout << e.what() << i << " extra handle_aom!\n";
      if (i == 1)
        return nullptr;
      // memory_limit >>= 1;
      // memory_limit *= 3;
      rt::need_draw = true;
      rt::draw();
      rt::handle_aom();
      rt::need_draw = false;
    }
  }
  rt::draw();
  auto ptr_on_object = reinterpret_cast<instance**>(ptr);
  *ptr_on_object     = inst;
  return reinterpret_cast<void*>(reinterpret_cast<size_t>(ptr_on_object) + 8);
}

extern "C" void __go(void (&func)()) {
  threads::Threads::instance().append(func);
}

extern "C" void __sleep(size_t n) {
  std::this_thread::sleep_for(std::chrono::seconds(n));
}