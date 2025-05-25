#pragma once

namespace AST {

struct Stmt;
struct TopLevel;
struct Expr;
struct TranslationUnit;
struct Const;
struct BinExp;
struct Var;
struct Ret;
struct TypeDeclaration;
struct Assign;
struct Function;
struct Call;
struct VarDecl;
enum class TypeClass;

// noooo clang format noooo....
// i don't wanna fix you but wtf is that terrorism
template <typename ReturnType> class ASTVisitor {

public:
  virtual ReturnType visit(AST::Stmt &stmt) = 0;
  virtual ReturnType visit(AST::Expr &expr) = 0;
  virtual ReturnType visit(AST::Const &c) = 0;
  virtual ReturnType visit(AST::BinExp &bin) = 0;
  virtual ReturnType visit(AST::Var &var) = 0;
  virtual ReturnType visit(AST::Ret &ret) = 0;
  virtual ReturnType visit(AST::Assign &as) = 0;
  virtual ReturnType visit(AST::Call &call) = 0;
  virtual ~ASTVisitor() {}
};

} // namespace AST
