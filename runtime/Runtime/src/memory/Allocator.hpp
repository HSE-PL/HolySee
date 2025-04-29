#pragma once
#include "alloca/Arena.hpp"
#include "alloca/heap/Heap.hpp"
#include <mutex>


class Allocator {

  std::recursive_mutex mutex_;

public:
  inline static void* heap_start = nullptr;
  Heap<Arena>         heap;
  // BitMap       emplaced_; // 4 accurate gc

  static Allocator& instance();
  Allocator();

  virtual ~Allocator() = default;

  virtual auto alloc(size_t object_size) -> ref;

  auto add_active(size_t index) -> Arena*;

  auto free(ref ptr) -> void;

  auto free_arena(Arena* a) -> void;

  auto revive(Arena* a) -> void;

  // 4 debug
  auto dump() const -> void;
};