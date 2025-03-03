#pragma once

#include "ir/pass/pass.hpp"

class TestPass : public BPass {
  void pass(Block &b);
};
