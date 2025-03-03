#pragma once

#include "fn.hpp"

struct Program {
  std::vector<Function> fns;
  Program(std::vector<Function> fns) : fns(fns) {}
  friend std::ostream &operator<<(std::ostream &o, const Program &fn);
};
