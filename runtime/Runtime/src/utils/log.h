#pragma once
#include <thread>

#ifdef DEBUG
#include <iostream>
#include <mutex>

static std::mutex log_mutex;

struct ThreadSafeCout {
  template <typename T>
  ThreadSafeCout& operator<<(const T& value) {
    std::lock_guard<std::mutex> lock(log_mutex);
    std::cout << std::this_thread::get_id() << ": \n" << value;
    return *this;
  }

  ThreadSafeCout& operator<<(std::ostream& (*manip)(std::ostream&)) {
    std::lock_guard<std::mutex> lock(log_mutex);
    std::cout << manip;
    return *this;
  }
};

static ThreadSafeCout log;

#else
class NullStream {
public:
  template <typename T>
  NullStream& operator<<(const T&) {
    return *this;
  }

  // NullStream& operator<<(std::ostream& (*)(std::ostream&)) {
  //   return *this;
  // }
};
static NullStream log;
#endif