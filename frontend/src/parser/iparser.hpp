#pragma once

#include "../lang/ast.hpp"
#include "../lexer/ilexer.hpp"
#include <memory>
#include <string_view>

class ParserException {
public:
  std::string error_msg;
  ParserException(std::string error) : error_msg(error) {}
};

class IParser {

public:
  virtual std::shared_ptr<Expr> parse(std::vector<Lexeme> &lexemes) = 0;
  virtual ~IParser() {};
};
