#pragma once
#include "Arena.hpp"
#include "Bitset.hpp"
#include "Region.hpp"
#include "heap/Heap.hpp"
#include <mutex>

class BitMap : Bitset {
  ref        from_;
  ref        to_;
  std::mutex mutex_;

  size_t map(size_t n) {
    assert(from_ <= n);
    assert(n < to_);
    return (n - from_) / 8;
  }

public:
  BitMap(ref from, ref to) : Bitset((to - from) / 64), from_(from), to_(to) {
  }

  void set(ref n);

  bool operator[](ref n);

  void clear();

  void clear(ref from, ref to);
};

class Allocator : public Heap<Arena> {

  std::recursive_mutex mutex_;

protected:
  const ref    start_;
  const size_t size_;
  BitMap       emplaced_;
  BitMap       marking_;

  std::vector<Region<Arena>*> regions_{};

public:
  Allocator(ref start, size_t size);

  ~Allocator() override = default;

  virtual ref alloc(size_t object_size);

  void add_active(size_t index);

  [[nodiscard]] Arena* arena_by_ptr(ref ptr) const;

  void free(ref ptr);

  void free_arena(Arena* a);
};
