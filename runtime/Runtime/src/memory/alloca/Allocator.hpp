#pragma once
#include "Arena.hpp"
#include "Region.hpp"
#include "heap/Heap.hpp"

class Allocator : public Heap<Arena> {
  const size_t start;

  std::vector<Region<Arena>> regions{};

protected:
  const size_t size;

public:
  Allocator(size_t start_heap, size_t size_heap);

  ~Allocator() override = default;

  virtual size_t alloc(size_t object_size);
};