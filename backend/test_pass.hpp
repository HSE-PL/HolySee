#pragma once

#include "ir/ir.hpp"

class TestPass : public LPass {
  void pass(Function &f);
};
