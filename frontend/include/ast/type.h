#ifndef HSEC_FRONTEND_AST_TYPE_H
#define HSEC_FRONTEND_AST_TYPE_H

#include <memory>
#include <vector>

#include "ast/node.h"

namespace hsec::frontend::ast {

struct BaseType : Node {};

using Type = std::shared_ptr<BaseType>;

struct Field : Node {
  Ident name;
  Type type;

  Field(Ident name, Type type) : name(name), type(type) {}

 protected:
  virtual void print(Printer& out) const override;
};

struct NamedType : BaseType {
  std::string name;

  NamedType(std::string name) : name(name) {}

 protected:
  virtual void print(Printer& out) const override;
};

struct StructType : BaseType {
  std::vector<Field> fields;

  StructType(std::vector<Field> fields) : fields(fields) {}

 protected:
  virtual void print(Printer& out) const override;
};

struct UnionType : BaseType {
  std::vector<Field> fields;

  UnionType(std::vector<Field> fields) : fields(fields) {}

 protected:
  virtual void print(Printer& out) const override;
};

}  // namespace hsec::frontend::ast

#endif
