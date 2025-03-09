#pragma once

#include "fn.hpp"
#include "streamers/iostreamer.hpp"
#include <iostream>
#include <memory>
#include <unordered_map>

class Program {
  friend class IOStreamer;

public:
  std::unordered_map<std::string, std::shared_ptr<Fn>> funcs;
  void accept(IOStreamer &io) { io.visit(*this); }
  void addFunc(std::shared_ptr<Fn> fn) { funcs.insert({fn->name(), fn}); }
};
