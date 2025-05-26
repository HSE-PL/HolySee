#pragma once

#include "ilexer.hpp"

class LexerImpl final : public ILexer {

public:
  virtual std::vector<Lexeme> lex(const std::string_view stringInput);
};
