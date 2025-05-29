#pragma once
#include "utils/defines.h"

#include <memory>
#include <thread>

struct Horoutine {
  std::shared_ptr<std::thread> routine;
  ref                          start_sp;
  std::shared_ptr<bool>        died;
};
