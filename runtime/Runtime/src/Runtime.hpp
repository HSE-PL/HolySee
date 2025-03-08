#pragma once
#include <stddef.h>

extern "C" void __rt_init(void*  __start,
                          void** spdptr);

extern "C" void* __halloc(size_t size);