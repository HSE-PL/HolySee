#pragma once

#include "../../ir_gen/json.hpp"
#include "../types/vtype.hpp"
#include "block.hpp"
#include <string>

struct Arg {
  VType type;
  Arg(nlohmann::json &value);
  std::string name;
};

class Function {
public:
  std::string name;
  std::vector<Arg> args;
  VType type;
  std::vector<Block> blocks;

  Function(nlohmann::json &fn);
  friend std::ostream &operator<<(std::ostream &o, const Function &fn);
};
