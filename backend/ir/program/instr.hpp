#pragma once

#include "../../ir_gen/json.hpp"
#include "../types/itype.hpp"
#include "../types/vtype.hpp"
#include "value.hpp"
#include <optional>
#include <ostream>
#include <string>
#include <vector>

class Instr {
public:
  IType op;
  VType type;
  std::optional<std::string> dest;
  std::vector<Value> args;

  Instr(nlohmann::json &instr);
  friend std::ostream &operator<<(std::ostream &o, const Instr &fn);
};
