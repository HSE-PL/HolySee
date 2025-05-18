#pragma once

#include "../ir/program.hpp"
#include "../lang/ast_visitor.hpp"

class ASTTranslator : public AST::ASTVisitor<std::shared_ptr<IR::Value>> {
  using VarPtr = std::shared_ptr<IR::Value>;
  /*FnBuilder cfn;*/
  /*BBuilder cblock;*/

public:
  /*ASTTranslator() : cfn(FnBuilder()), cblock(BBuilder("")) {}*/
  IR::Program translate(AST::TranslationUnit &unit);
  VarPtr visit(AST::Stmt &stmt) = 0;
  VarPtr visit(AST::TopLevel &tl) = 0;
  VarPtr visit(AST::Expr &expr) = 0;
  VarPtr visit(AST::TranslationUnit &tu) = 0;
  VarPtr visit(AST::Const &c) = 0;
  VarPtr visit(AST::BinExp &bin) = 0;
  VarPtr visit(AST::Var &var) = 0;
  VarPtr visit(AST::Ret &ret) = 0;
  VarPtr visit(AST::TypeDeclaration &td) = 0;
  VarPtr visit(AST::Assign &as) = 0;
  VarPtr visit(AST::Function &fn) = 0;
  VarPtr visit(AST::Call &call) = 0;
};
