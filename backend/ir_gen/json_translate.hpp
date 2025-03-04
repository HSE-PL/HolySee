#pragma once

#include "../ir/pass/pass_manager.hpp"

class JsonTranslator {
public:
  static PassManager json2pm(std::string &json);
};
