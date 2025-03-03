#pragma once

#include "../types/vtype.hpp"
#include <string>
#include <variant>

struct Value {
  VType type;
  std::variant<std::pair<std::string, std::string>, std::string, int> val;
  Value(VType type,
        std::variant<std::pair<std::string, std::string>, std::string, int> val)
      : type(type), val(val) {}
};
