#pragma once
#include <stddef.h>
void* __dso_handle = nullptr;

typedef size_t Metadata;

typedef struct {
  size_t   size;
  MetaData md[];
} TypeTable;

extern "C" void __rt_init(void (&__start)(), void** spdptr, void* sp, TypeTable* tt);

extern "C" void* __halloc(size_t size);
