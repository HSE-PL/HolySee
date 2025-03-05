#pragma once

#include "pass.hpp"

class PassManager {
  friend class JsonTranslator;

  Program program_;
  PassManager(Program p) : program_(p) {}

public:
  Program &program() { return program_; }
  void pass(BPass &pass) {
    for (auto &fn : program_.fns) {
      for (auto &blocks : fn.blocks) {
        pass.pass(blocks);
      }
    }
  }
  void pass(LPass &pass) {
    for (auto &fn : program_.fns) {
      pass.pass(fn);
    }
  }
  void pass(GPass &pass) { pass.pass(program_); }
  friend std::ostream &operator<<(std::ostream &o, const PassManager &fn);
};
