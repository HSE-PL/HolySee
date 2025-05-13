#pragma once

#include "iparser.hpp"
#include <memory>
#include <vector>
class ParserImpl : public IParser {

public:
  std::optional<std::shared_ptr<TopLevel>>
  parseTopLevel(std::vector<Lexeme> &lexems);
  virtual std::shared_ptr<Expr> parse(std::vector<Lexeme> &lexemes);
};
