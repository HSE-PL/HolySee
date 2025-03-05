#pragma once

#include "../ir_translator.hpp"

class JsonTranslator final : public IrTranslator<std::string> {

public:
  JsonTranslator() = default;
  PassManager toIR(std::string);
  std::ostream &toStream(std::ostream &stream, const PassManager &pm);
};
