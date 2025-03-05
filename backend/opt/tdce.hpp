#pragma once

#include "../ir/pass/pass.hpp"

class TDCEPass final : public LPass {

public:
  TDCEPass() = default;
  void pass(Block &f);
};
