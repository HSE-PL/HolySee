#pragma once

#include "iparser.hpp"
#include <memory>
#include <vector>

namespace AST {

class ParserImpl : public IParser {

public:
  std::optional<std::shared_ptr<AST::Stmt>>
  parseStmt(std::vector<Lexeme> &lexems);
  AST::TranslationUnit parse(std::vector<Lexeme> &lexemes);
};
} // namespace AST
