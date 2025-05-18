#include "translator.hpp"
#include "../ir/factory/ifactory.hpp"

IR::Program ASTTranslator::translate(AST::TranslationUnit &unit) {
  //
}

std::shared_ptr<IR::Value> ASTTranslator::visit(AST::Const &c) {}
