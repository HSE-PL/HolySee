#pragma once
#include "Arena.hpp"
#include "Bitset.hpp"
#include "Region.hpp"
#include "heap/Heap.hpp"

class BitMap : Bitset {
  uint64_t   from_;
  uint64_t   to_;
  std::mutex mutex_;

  size_t map(size_t n) {
    assert(from_ <= n);
    assert(n < to_);
    return (n - from_) / 8;
  }

public:
  BitMap(uint64_t from, uint64_t to)
      : Bitset(from - to), from_(from), to_(to) {}

  void set(size_t n);

  size_t get(size_t n);

  void clear();

  void clear(size_t from, size_t to);
};

class Allocator : public Heap<Arena> {
  const size_t start_;

  std::vector<Region<Arena>> regions_{};
  std::recursive_mutex       mutex_;

protected:
  const size_t size_;
  BitMap       emplaced_;

public:
  Allocator(size_t start, size_t size);

  ~Allocator() override = default;

  virtual size_t alloc(size_t object_size);

  void add_active(size_t index);

  [[nodiscard]] Arena*
  arena_by_ptr(size_t ptr) const;

  void free(size_t ptr);

  void free_arena(Arena* a);
};
