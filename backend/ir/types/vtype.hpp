#pragma once

#include <string>
#include <unordered_map>

enum class VType {
  Int,
  Bool,
  Unit,
  Ref,
  Refs,
  Func,
  VArg,
  Label,
  Branches,
};

VType string2type(std::string type);

extern std::unordered_map<VType, std::string> t2s;
extern std::unordered_map<std::string, VType> s2t;
