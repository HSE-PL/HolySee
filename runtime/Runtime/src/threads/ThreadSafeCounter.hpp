#pragma once
#include "utils/defines.h"

#include <assert.h>
#include <mutex>
class ThreadSafeCounter {
  size_t     counter_;
  std::mutex mutex_;

public:
  ThreadSafeCounter() = default;

  size_t operator++() {
    guard(mutex_);
    return ++counter_;
  }

  void operator--() {
    guard(mutex_);
    assert(counter_);
    counter_--;
  }

  size_t operator*() {
    guard(mutex_);
    return counter_;
  }

  void clear() {
    guard(mutex_);
    counter_ = 0;
  }
};