#pragma once

#include "fn.hpp"
#include <memory>
#include <unordered_map>

class Program {
  std::unordered_map<std::string, std::shared_ptr<Fn>> funcs;

public:
  void addFunc(std::shared_ptr<Fn> fn) { funcs.insert({fn->name(), fn}); }
};
