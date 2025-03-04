#pragma once

#include "pass.hpp"

class PassManager {
  friend class JsonTranslator;

  Program program;
  PassManager(Program p) : program(p) {}

public:
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
