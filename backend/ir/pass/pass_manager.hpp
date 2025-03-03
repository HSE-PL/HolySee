#pragma once

#include "../program/program.hpp"
#include "pass.hpp"

class PassManager {
  Program program;

public:
  PassManager(Program p) : program(p) {}

  void pass(BPass &pass) {
    for (auto &fn : program.fns) {
      for (auto &blocks : fn.blocks) {
        pass.pass(blocks);
      }
    }
  }
  void pass(LPass &pass) {
    for (auto &fn : program.fns) {
      pass.pass(fn);
    }
  }
  void pass(GPass &pass) { pass.pass(program); }
  friend std::ostream &operator<<(std::ostream &o, const PassManager &fn);
};
