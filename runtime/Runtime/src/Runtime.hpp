#pragma once
#include <stddef.h>
void* __dso_handle = nullptr;

extern "C" void __rt_init(void* __start, void** spdptr);

extern "C" void* __halloc(size_t size);