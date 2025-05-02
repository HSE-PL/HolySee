#ifndef HSEC_FRONTEND_AST_DECL_H
#define HSEC_FRONTEND_AST_DECL_H

#include "ast/node.h"
#include "ast/type.h"

namespace hsec::frontend::ast {

struct BaseDecl : Node {};

using Decl = std::shared_ptr<BaseDecl>;

// class ConstDecl : public Node, public TopLevelDecl {};

// class VarDecl : public Node, public TopLevelDecl {};

// class FunDecl : public Node, public TopLevelDecl {};

// class NamespaceDecl : public Node, public TopLevelDecl {};

struct TypeDecl : BaseDecl {
  Ident name;
  Type type;

  TypeDecl(std::string name, Type type) : name(name), type(type) {}

 protected:
  virtual void print(Printer& out) const override;
};

}  // namespace hsec::frontend::ast

#endif
