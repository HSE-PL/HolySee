#pragma once
#include "utils/defines.h"
#include "utils/log.h"
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

  fn begin() {
    return v_.begin();
  }

  fn end() {
    return v_.end();
  }

  fn push(auto item) {
    guard(mutex_);
    log << "try to push " << v_.capacity() << " " << v_.size() << "\n";
    v_.push_back(item);
    log << "push complete\n";
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

  fn clear() {
    guard(mutex_);
    log << v_.capacity() << " " << v_.size() << "\n";
    v_ = std::vector<T>();
    log << "1\n";
    // throw std::runtime_error("");
    // log << "exper1\n";
    // T tmp;
    // v_.push_back(tmp);
    // v_.pop_back();
    // log << "exper2\n";
  }
};