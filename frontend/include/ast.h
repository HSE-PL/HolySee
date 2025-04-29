#ifndef HSEC_FRONTEND_AST_H
#define HSEC_FRONTEND_AST_H

#include <memory>
#include <ostream>
#include <vector>

namespace hsec::frontend::ast {

struct Node {
  virtual ~Node() {}

 protected:
  friend std::ostream& operator<<(std::ostream& os, const Node& node) {
    return node.print(os);
  }

  virtual std::ostream& print(std::ostream& out) const = 0;
};

class ConstDecl;
class VarDecl;

class FunDecl;
class TypeDecl;
class NamespaceDecl;

class NamedType;
class StructType;

struct Type {
  struct Visitor {
    virtual void accept(NamedType&) {};
    virtual void accept(StructType&) {};
  };

  virtual void visit(Visitor&) = 0;
  void visit(Visitor&& visitor) { visit(visitor); };

  virtual ~Type() {};

  // FORCED (printable)
 protected:
  friend std::ostream& operator<<(std::ostream& os, const Type& node) {
    return node.print(os);
  }

  virtual std::ostream& print(std::ostream& out) const = 0;
};

class NamedType : public Node, public Type {
  std::string name;

 public:
  NamedType(std::string name) : name(name) {}

  void visit(Visitor& visitor) override { visitor.accept(*this); }

 protected:
  std::ostream& print(std::ostream&) const override;
};

class Field : public Node {
  std::string name;
  std::shared_ptr<Type> type;  // FORCED (shared_ptr)

 public:
  Field(std::string name, std::shared_ptr<Type> type)
      : name(name), type(type) {}

 protected:
  virtual std::ostream& print(std::ostream& out) const;
};

class StructType : public Node, public Type {
  std::vector<Field> fields;

 public:
  StructType(std::vector<Field> fields) : fields(fields) {}

  void visit(Visitor& visitor) override { visitor.accept(*this); }

 protected:
  std::ostream& print(std::ostream&) const override;
};

struct Decl {
  struct Visitor {
    virtual void accept(ConstDecl&) {};
    virtual void accept(VarDecl&) {};
  };

  virtual void visit(Visitor&) = 0;
  void visit(Visitor&& visitor) { visit(visitor); };
};

struct TopLevelDecl {
  struct Visitor : Decl::Visitor {
    virtual void accept(ConstDecl&) override {};
    virtual void accept(VarDecl&) override {};
    virtual void accept(FunDecl&) {};
    virtual void accept(NamespaceDecl&) {};
    virtual void accept(TypeDecl&) {};
  };

  virtual void visit(Visitor&) = 0;
  void visit(Visitor&& visitor) { visit(visitor); };
};

class ConstDecl : public Node, public TopLevelDecl {
  void visit(TopLevelDecl::Visitor& visitor) override { visitor.accept(*this); }
};

class VarDecl : public Node, public TopLevelDecl {
  void visit(TopLevelDecl::Visitor& visitor) override { visitor.accept(*this); }
};

class FunDecl : public Node, public TopLevelDecl {
  void visit(TopLevelDecl::Visitor& visitor) override { visitor.accept(*this); }
};

class NamespaceDecl : public Node, public TopLevelDecl {
  void visit(TopLevelDecl::Visitor& visitor) override { visitor.accept(*this); }
};

class TypeDecl : public Node, public TopLevelDecl {
  std::string name;
  std::shared_ptr<Type> type;

 public:
  TypeDecl(std::string name, std::shared_ptr<Type> type)
      : name(name), type(type) {}

  void visit(TopLevelDecl::Visitor& visitor) override { visitor.accept(*this); }

  virtual std::ostream& print(std::ostream& out) const override;
};

};  // namespace hsec::frontend::ast

#endif
