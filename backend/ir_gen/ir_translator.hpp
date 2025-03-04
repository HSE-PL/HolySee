#pragma once

#include "../ir/pass/pass_manager.hpp"

template <typename Spec> class IrTranslator {

public:
  virtual PassManager toIR(Spec input) = 0;
  /*virtual std::ostream toSpec(std::ostream &stream, const PassManager &pm) =
   * 0;*/
};
