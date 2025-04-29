#pragma once
#include <stddef.h>
#include <utils/defines.h>
namespace rt {
  size_t static max_heap_size = 256 * 1024 * 1024 * 1024;
}

extern "C" void __rt_init(void (&__start)(), void** spdptr, void* sp, instance*,
                          instance*, size_t max_heap_size);

extern "C" void* __halloc(instance*);
