#pragma once

#include "../types/itype.hpp"
#include "../types/vtype.hpp"
#include "value.hpp"
#include <iostream>
#include <optional>
#include <vector>

class Instr {
public:
  IType op;
  VType type;
  std::optional<std::string> dest;
  std::vector<Value> args;

  friend std::ostream &operator<<(std::ostream &o, const Instr &fn);
};
