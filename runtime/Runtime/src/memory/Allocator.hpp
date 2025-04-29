#pragma once
#include "alloca/Arena.hpp"
#include "alloca/Region.hpp"
#include "alloca/heap/Heap.hpp"
#include <mutex>


class Allocator {

  std::recursive_mutex mutex_;

public:
  Heap<Arena> heap;
  // BitMap       emplaced_; // 4 accurate gc

  static Allocator& instance();
  Allocator();

  virtual ~Allocator() = default;

  virtual fn alloc(size_t object_size)->ref;

  fn add_active(size_t index)->Arena*;

  fn free(ref ptr)->void;

  fn free_arena(Arena* a)->void;

  fn revive(Arena* a)->void;

  // 4 debug
  fn dump() const->void;
};