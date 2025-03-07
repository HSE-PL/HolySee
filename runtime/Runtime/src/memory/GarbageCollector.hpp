#pragma once
#include "alloca/Allocator.hpp"
#include <csignal>
#include <sys/ucontext.h>

class GarbageCollector final : public Allocator {
  size_t memory;

public:
  GarbageCollector(size_t start_heap,
                   size_t size_heap);

  void GC();
  void cleaning(siginfo_t*  info,
                ucontext_t* context);

  size_t alloc(size_t) override;
};
