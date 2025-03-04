#pragma once

#include "fn.hpp"
#include <vector>

class Program {
  friend class JsonTranslator;

  Program(std::vector<Function> fns) : fns(fns) {}

public:
  std::vector<Function> fns;
  friend std::ostream &operator<<(std::ostream &o, const Program &fn);
};
