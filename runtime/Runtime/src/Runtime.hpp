#pragma once
#include <stddef.h>
static void* __dso_handle = nullptr;

typedef size_t MetaData;

typedef struct {
  size_t   size;
  MetaData md[];
} TypeTable;

extern "C" void __rt_init(void (&__start)(), void** spdptr, void* sp, TypeTable* tt);

extern "C" void* __halloc(size_t type);
