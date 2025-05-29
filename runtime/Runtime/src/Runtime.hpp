#pragma once
#include <stddef.h>
#include <utils/defines.h>
namespace rt {
  void draw();
}

extern "C" void __rt_init(void (&__start)(...), void** spdptr, void* sp,
                          instance*, instance*, size_t max_heap_size);

extern "C" void* __halloc(instance*);