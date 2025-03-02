#pragma once
#include "Arena.hpp"
#include "Region.hpp"
#include "heap/Heap.hpp"

class Allocator : public Heap<Arena> {
  const size_t start;
  const size_t size;

  std::vector<Region<Arena>> regions;

  Allocator(size_t start_heap, size_t size_heap);

  size_t alloc(size_t object_size);
};