#pragma once

#include "ir/pass/pass.hpp"

class TestPass : public LPass {
  void pass(Block &b);
};
