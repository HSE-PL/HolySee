#pragma once
#include "Arena.hpp"
#include "Bitset.hpp"
#include "Region.hpp"
#include "heap/Heap.hpp"
#include <mutex>

class BitMap : Bitset {
  ref from_;
  ref to_;

  std::recursive_mutex mutex_;

  size_t map(size_t n) {
    assert(from_ <= n);
    assert(n < to_);
    return (n - from_) / 8;
  }

public:
  BitMap(ref from, ref to) : Bitset((to - from) / 64), from_(from), to_(to) {
  }

  fn set(ref n)->void;

  fn check_and_set(ref n)->bool;

  fn clear()->void;

  fn clear(ref from, ref to)->void;
};

class Allocator {

  std::recursive_mutex mutex_;

public:
  Heap<Arena>  heap_;
  const ref    start_;
  const size_t size_;
  BitMap       emplaced_;
  BitMap       marking_;

  std::vector<Region<Arena>*> regions_{};

  Allocator(ref start, size_t size);

  virtual ~Allocator() = default;

  virtual fn alloc(size_t object_size)->ref;

  fn add_active(size_t index)->void;

  [[nodiscard]] Arena* arena_by_ptr(ref ptr) const;

  fn free(ref ptr)->void;

  fn free_arena(Arena* a)->void;

  fn revive(Arena* a)->void;
};