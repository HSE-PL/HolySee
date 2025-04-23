#pragma once
#include <safepoints/Safepoint.hpp>
#include <stddef.h>
#include <utils/defines.h>
static void* __dso_handle = nullptr;

const size_t min_heap = sp::pagesize * (1 << 5);

extern "C" void __rt_init(void (&__start)(), void** spdptr, void* sp);

extern "C" void* __halloc(instance*);