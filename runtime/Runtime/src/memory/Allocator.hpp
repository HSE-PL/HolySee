#pragma once
#include "alloca/Arena.hpp"
#include "alloca/heap/Heap.hpp"
#include <mutex>


class Allocator {

  std::recursive_mutex mutex_;

public:
  Heap<Arena> heap;

  static Allocator& instance();
  Allocator();
  ~Allocator() = default;

  [[nodiscard]] auto alloc(size_t object_size) -> ref;

  [[nodiscard]] auto add_active(size_t index) -> Arena*;

  auto free(ref ptr) -> void;

  auto free_arena(Arena* a) -> void;

  auto revive(Arena* a) -> void;

  // 4 debug
  auto dump() const -> void;
};