#pragma once
#include "utils/defines.h"
#include <mutex>
#include <vector>
template <typename T>
class ThreadSafeVector {
  std::mutex     mutex_;
  std::vector<T> v_;

public:
  ThreadSafeVector() = default;

  auto begin() {
    return v_.begin();
  }

  auto end() {
    return v_.end();
  }

  void push(T item) {
    guard(mutex_);
    v_.push_back(item);
  }

  T operator[](int index) {
    guard(mutex_);
    return v_[index];
  }

  size_t size() {
    guard(mutex_);
    return v_.size();
  }

  void pop() {
    guard(mutex_);
    v_.pop_back();
  }

  T back() {
    guard(mutex_);
    return v_.back();
  }
};