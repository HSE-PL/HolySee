#pragma once
#include "MemoryManager.hpp"
#include "tbb/concurrent_queue.h"

class GarbageCollector {

  tbb::concurrent_queue<ref> queue_for_marked_;

  inteval<instance> inteval_;


  auto static how_many_ref(instance* p) -> size_t;
  auto how_many_ref(ref p) -> size_t;

  auto ref_is_real(ref ptr) -> bool;

  void make_root_and_tracing(ref ssp);
  auto tracing() -> void;

  std::atomic_uint_fast64_t num_of_cleaning = 0;

  auto cleaning() -> void;
  auto marking(ref ptr) -> void;


public:
  GarbageCollector(instance*, instance*);

  auto gogc(ref ssp) -> void;
  void GC();
};
