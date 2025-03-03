#pragma once

#include "../program/program.hpp"

class Pass {};

class GPass : public Pass {
public:
  virtual void pass(Program &p) = 0;
};
class LPass : public Pass {
public:
  virtual void pass(Function &f) = 0;
};
class BPass : public Pass {
public:
  virtual void pass(Block &f) = 0;
};
