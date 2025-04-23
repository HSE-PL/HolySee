#pragma once
#include "utils/defines.h"
#include <mutex>
#include <optional>
#include <vector>
template <typename T>
class ThreadSafeVector {
  std::mutex mutex_;

public:
  std::vector<T> v_;

  ThreadSafeVector() = default;

  fn begin() {
    return v_.begin();
  }

  fn end() {
    return v_.end();
  }

  fn push(auto item) {
    guard(mutex_);
    v_.push_back(item);
  }

  fn operator[](int index)->T {
    guard(mutex_);
    return v_[index];
  }

  fn size()->size_t {
    guard(mutex_);
    return v_.size();
  }

  fn pop()->std::optional<T> {
    guard(mutex_);
    if (v_.empty())
      return std::nullopt;
    let b = v_.back();
    v_.pop_back();
    return b;
  }
};