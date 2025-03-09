#pragma once

#include "fn.hpp"
#include "streamers/iostreamer.hpp"
#include <memory>
#include <unordered_map>

class Program {
  friend class IOStreamer;
  std::unordered_map<std::string, std::shared_ptr<Fn>> funcs;

public:
  void accept(IOStreamer &io) { io.visit(*this); }
  void addFunc(std::shared_ptr<Fn> fn) { funcs.insert({fn->name(), fn}); }
};
