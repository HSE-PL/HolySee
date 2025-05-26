#pragma once

#include "../ir/blocks/bbuilder.hpp"
#include "../ir/fnbuilder.hpp"
#include "../ir/program.hpp"
#include "../lang/ast_visitor.hpp"

#define todo assert(false && "TODO");

struct TranslatorContext {
  AST::TranslationUnit *tu;
};

class ASTTranslator : public AST::ASTVisitor<std::shared_ptr<IR::Value>> {
  using ValPtr = std::shared_ptr<IR::Value>;
  IR::FnBuilder cfn;
  IR::BBuilder cblock;
  IR::IFactory ifactory;
  IR::VFactory vfactory;
  size_t curTemp;
  std::shared_ptr<IR::Ref> createTemp(AST::TypeClass tc);
  void endBlock(std::string new_block);
  TranslatorContext ctx;

public:
  ASTTranslator() {}
  IR::Program translate(AST::TranslationUnit &unit);
  ValPtr visit(AST::Stmt &stmt);
  ValPtr visit(AST::VarDecl &vd) { return vfactory.createUnit(); }
  ValPtr visit(AST::Expr &expr);
  ValPtr visit(AST::If &expr);
  ValPtr visit(AST::Const &c);
  ValPtr visit(AST::BinExp &bin); // TODO
  ValPtr visit(AST::Var &var);
  ValPtr visit(AST::Ret &ret);
  ValPtr visit(AST::If &iff, std::string end);
  ValPtr visit(AST::Assign &as);
  std::shared_ptr<IR::Fn> visitFn(AST::Function &fn); // TODO
  ValPtr visit(AST::Call &call);
};
