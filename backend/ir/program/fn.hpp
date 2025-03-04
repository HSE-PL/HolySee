#pragma once

#include "../types/vtype.hpp"
#include "block.hpp"
#include <vector>

struct Arg {
  VType type;
  std::string name;
};

class Function {
  friend class JsonTranslator;

  Function() = default;

public:
  std::string name;
  std::vector<Arg> args;
  VType type;
  std::vector<Block> blocks;

  friend std::ostream &operator<<(std::ostream &o, const Function &fn);
};
