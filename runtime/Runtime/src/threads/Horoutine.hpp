#pragma once
#include <thread>

struct Horoutine {
  std::thread* routine;
  size_t       start_sp;
};
