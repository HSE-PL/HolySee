#pragma once
#include "defines.h"
#include "log.h"
#include <mutex>
#include <optional>
#include <vector>
template <typename T>
class ThreadSafeVector {
  std::mutex mutex_;

public:
  std::vector<T> v_;

  ThreadSafeVector()  = default;
  ~ThreadSafeVector() = default;

  auto begin() {
    return v_.begin();
  }

  auto end() {
    return v_.end();
  }

  auto push(auto item) {
    guard(mutex_);
    v_.push_back(item);
  }

  auto operator[](int index) -> T {
    guard(mutex_);
    return v_[index];
  }

  auto size() -> size_t {
    guard(mutex_);
    return v_.size();
  }

  auto pop() -> std::optional<T> {
    guard(mutex_);
    if (v_.empty())
      return std::nullopt;
    auto b = v_.back();
    v_.pop_back();
    return b;
  }

  auto clear() {
    guard(mutex_);
    v_.clear();
    v_.shrink_to_fit();
  }

  auto empty() -> bool {
    guard(mutex_);
    return v_.empty();
  }
};