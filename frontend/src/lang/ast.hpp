#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class Expr;

class TopLevel {
public:
  virtual std::string toString() = 0;
  virtual ~TopLevel() {};
};

class Stmt {
public:
  virtual std::string toString() = 0;
  virtual ~Stmt() {};
};

class Expr : public Stmt {};

// ha-ha typeclass haha not the one from haskell tho
enum class TypeClass {
  Int,
  Bool,
  Void,
  Custom,
};

enum class BinOp {
  Add,
  Sub,
  Mul,
  Div,
};

struct TypeEntry {
  std::string name;
  TypeClass tclass;
  bool isPrimitive() {
    return tclass == TypeClass::Int || tclass == TypeClass::Bool;
  }
  std::string toString() { return name; }
};

class Const : public Expr {
  int value;

public:
  Const(int value) : value(value) {}
  virtual std::string toString() { return std::to_string(value); }
};

class BinExp : public Expr {
  std::shared_ptr<Expr> lhs;
  std::shared_ptr<Expr> rhs;
  BinOp op;

public:
  BinExp(std::shared_ptr<Expr> lhs, std::shared_ptr<Expr> rhs, BinOp op)
      : lhs(lhs), rhs(rhs), op(op) {}
  virtual std::string toString();
};

class Var : public Expr {
  std::string id;
  TypeEntry type;

public:
  Var(std::string id, TypeEntry type) : id(id), type(type) {}
  virtual std::string toString() { return id + " " + type.name; }
};

struct TypeDeclaration : public TopLevel {
  std::string type;
  std::vector<std::shared_ptr<Var>> fields;
  TypeDeclaration(std::string type) : type(type) {}
  std::string toString() {
    std::string fieldsString{};
    for (auto &&field : fields) {
      fieldsString += "  " + field->toString() + "\n";
    }
    return "struct " + type + " {\n" + fieldsString + "}";
  }
};

enum class Predicate {
  LE,
  GE,
  LT,
  GT,
  EQ,
  NEQ,
};

// TODO: delete if i don't see any further usage
class Cond : public Expr {
  std::shared_ptr<Expr> lhs;
  std::shared_ptr<Expr> rhs;
  Predicate p;

public:
  Cond(std::shared_ptr<Expr> lhs, std::shared_ptr<Expr> rhs, Predicate p)
      : lhs(lhs), rhs(rhs), p(p) {}
  virtual std::string toString();
};

class If : public Expr {
  std::shared_ptr<Expr> cond;
  std::shared_ptr<Expr> tbranch;
  std::shared_ptr<Expr> fbranch;

public:
  If(std::shared_ptr<Cond> cond, std::shared_ptr<Expr> tbranch,
     std::shared_ptr<Expr> fbranch)
      : cond(cond), tbranch(tbranch), fbranch(fbranch) {}
  virtual std::string toString() {
    return "(if " + cond->toString() + tbranch->toString() + " " +
           fbranch->toString() + ")";
  }
};

class Let : public Expr {
  std::string id;
  std::shared_ptr<Expr> given;
  std::shared_ptr<Expr> expression;

public:
  Let(std::string id, std::shared_ptr<Expr> given,
      std::shared_ptr<Expr> expression)
      : id(id), given(given), expression(expression) {}

  virtual std::string toString() {
    return "(let " + id + " " + given->toString() + " in " +
           expression->toString() + ")";
  }
};

class Function : public TopLevel {
  std::string id;
  TypeEntry type;
  std::vector<std::shared_ptr<Var>> params;
  std::vector<std::shared_ptr<Stmt>> body;

public:
  Function(std::string id, TypeEntry type,
           std::vector<std::shared_ptr<Var>> params,
           std::vector<std::shared_ptr<Stmt>> body)
      : id(id), type(type), params(params), body(body) {}
  Function(const Function &other) = default;
  virtual std::string toString() {
    std::string paramsString{};
    std::string bodyString{};
    for (auto &&param : params) {
      paramsString += param->toString() + ", ";
    }
    for (auto &&stmt : body) {
      bodyString += "    " + stmt->toString() + "\n";
    }
    if (params.size() > 0) {
      paramsString.pop_back();
      paramsString.pop_back();
    }
    return "fun " + id + "(" + paramsString + ") " + type.toString() + "  {\n" +
           bodyString + "}";
  }
};

class Call : public Expr {
  std::string fn;
  std::vector<std::shared_ptr<Expr>> args;

public:
  Call(std::string fn, std::vector<std::shared_ptr<Expr>> args)
      : fn(fn), args(args) {}
  virtual std::string toString() {
    std::string argsString{};
    for (auto &&arg : args) {
      argsString += arg->toString() + ", ";
    }
    if (args.size() > 0) {
      argsString.pop_back();
      argsString.pop_back();
    }
    return fn + "(" + argsString + ")";
  }
};
