#pragma once

namespace AST {

class Stmt;
class TopLevel;
class Expr;
struct TranslationUnit;
class Const;
class BinExp;
struct Var;
class Ret;
struct TypeDeclaration;
class Assign;
struct Function;
class Call;

// noooo clang format noooo....
// i don't wanna fix you but wtf is that terrorism
template <typename ReturnType> class ASTVisitor {

public:
  virtual ReturnType visit(AST::Stmt &stmt) = 0;
  virtual ReturnType visit(AST::TopLevel &tl) = 0;
  virtual ReturnType visit(AST::Expr &expr) = 0;
  virtual ReturnType visit(AST::TranslationUnit &tu) = 0;
  virtual ReturnType visit(AST::Const &c) = 0;
  virtual ReturnType visit(AST::BinExp &bin) = 0;
  virtual ReturnType visit(AST::Var &var) = 0;
  virtual ReturnType visit(AST::Ret &ret) = 0;
  virtual ReturnType visit(AST::TypeDeclaration &td) = 0;
  virtual ReturnType visit(AST::Assign &as) = 0;
  virtual ReturnType visit(AST::Function &fn) = 0;
  virtual ReturnType visit(AST::Call &call) = 0;
  virtual ~ASTVisitor() {}
};

} // namespace AST
