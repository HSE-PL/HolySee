#pragma once

#include "../ast_translator/translator.hpp"
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace AST {

class Expr;
struct TranslationUnit;
struct TypeDeclaration;
struct Function;

class TopLevel {
public:
  virtual std::string toString() = 0;
  virtual std::shared_ptr<IR::Value> accept(ASTTranslator &translator) = 0;
  // WHY can't i do template and virtual
  // WTF
  // what's the point of this language
  // it's legitimately useless whenever i have actual problems
  /*template <typename Return>*/
  /*virtual Return accept(ASTVisitor<Return>& visitor) {}*/
  virtual ~TopLevel() {};
};

class Stmt {
public:
  virtual std::string toString() = 0;
  virtual std::shared_ptr<IR::Value> accept(ASTTranslator &translator) = 0;
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
  virtual std::shared_ptr<IR::Value> accept(ASTTranslator &translator) {
    return translator.visit(*this);
  }
  virtual std::string toString() { return std::to_string(value); }
};

class BinExp : public Expr {
  std::shared_ptr<Expr> lhs;
  std::shared_ptr<Expr> rhs;
  BinOp op;

public:
  virtual std::shared_ptr<IR::Value> accept(ASTTranslator &translator) {
    return translator.visit(*this);
  }
  BinExp(std::shared_ptr<Expr> lhs, std::shared_ptr<Expr> rhs, BinOp op)
      : lhs(lhs), rhs(rhs), op(op) {}
  virtual std::string toString();
};

struct Var : public Expr {
  std::string id;
  TypeEntry type;

public:
  virtual std::shared_ptr<IR::Value> accept(ASTTranslator &translator) {
    return translator.visit(*this);
  }
  Var(std::string id, TypeEntry type) : id(id), type(type) {}
  virtual std::string toString() { return id + " " + type.name; }
};

class Ret : public Stmt {
  std::optional<std::shared_ptr<Expr>> retExpr;

public:
  virtual std::shared_ptr<IR::Value> accept(ASTTranslator &translator) {
    return translator.visit(*this);
  }
  Ret(std::shared_ptr<Expr> expr) : retExpr(expr) {}
  Ret() : retExpr(std::nullopt) {}
  virtual std::string toString() {
    if (retExpr.has_value()) {
      return "ret " + (*retExpr)->toString();
    } else {
      return "ret _";
    }
  }
};

struct TypeDeclaration : public TopLevel {
  std::string type;
  std::vector<std::shared_ptr<Var>> fields;
  virtual std::shared_ptr<IR::Value> accept(ASTTranslator &translator) {
    return translator.visit(*this);
  }
  TypeDeclaration(std::string type) : type(type) {}
  std::string toString() {
    std::string fieldsString{};
    for (auto &&field : fields) {
      fieldsString += "  " + field->toString() + "\n";
    }
    return "struct " + type + " {\n" + fieldsString + "}";
  }
};

class Assign : public Stmt {
  std::shared_ptr<Var> var;
  std::shared_ptr<Expr> expr;

public:
  virtual std::shared_ptr<IR::Value> accept(ASTTranslator &translator) {
    return translator.visit(*this);
  }
  Assign(std::shared_ptr<Var> var, std::shared_ptr<Expr> expr)
      : var(var), expr(expr) {}

  std::string toString() { return var->toString() + " = " + expr->toString(); }
};

class VarDecl : public Stmt {
  std::vector<std::shared_ptr<Var>> vars;

public:
  virtual std::shared_ptr<IR::Value> accept(ASTTranslator &translator) {
    return translator.visit(*this);
  }
  VarDecl(std::vector<std::shared_ptr<Var>> vars) : vars(vars) {}
  std::string toString() {
    std::string varNames{};
    for (auto &&var : vars) {
      varNames += var->toString() + ", ";
    }
    if (vars.size() > 0) {
      varNames.pop_back();
      varNames.pop_back();
    }

    return "var " + varNames;
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
  virtual std::shared_ptr<IR::Value> accept(ASTTranslator &translator) {
    return translator.visit(*this);
  }
  If(std::shared_ptr<Cond> cond, std::shared_ptr<Expr> tbranch,
     std::shared_ptr<Expr> fbranch)
      : cond(cond), tbranch(tbranch), fbranch(fbranch) {}
  virtual std::string toString() {
    return "(if " + cond->toString() + tbranch->toString() + " " +
           fbranch->toString() + ")";
  }
};

struct Function : public TopLevel {
  std::string id;
  TypeEntry type;
  std::vector<std::shared_ptr<Var>> params;
  std::vector<std::shared_ptr<Stmt>> body;

public:
  virtual std::shared_ptr<IR::Value> accept(ASTTranslator &translator) {
    return translator.visit(*this);
  }
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
  virtual std::shared_ptr<IR::Value> accept(ASTTranslator &translator) {
    return translator.visit(*this);
  }
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

struct TranslationUnit {
  std::unordered_map<std::string, TypeEntry> types = {
      {"int", TypeEntry("int", TypeClass::Int)},
      {"bool", TypeEntry("bool", TypeClass::Bool)}};
  std::unordered_map<std::string, std::shared_ptr<TypeDeclaration>> typeDecls;
  std::unordered_map<std::string, std::shared_ptr<Function>> funs;

  void addFn(std::string name, std::shared_ptr<Function> fn) {
    funs.insert({name, fn});
  }
  void addType(std::string name, TypeEntry type) { types.insert({name, type}); }
  void addTypeDecl(std::string name, std::shared_ptr<TypeDeclaration> type) {
    typeDecls.insert({name, type});
  }
  std::optional<std::shared_ptr<Function>> fnLookup(std::string name) {
    if (funs.contains(name)) {
      return funs[name];
    }
    return std::nullopt;
  }
  std::optional<TypeEntry> typeLookup(std::string name) {
    if (types.contains(name)) {
      return types[name];
    }
    return std::nullopt;
  }
  std::string toString() {
    std::string res{};
    res += "Types:\n";
    for (auto &&type : typeDecls) {
      res += type.second->toString() + "\n";
    }
    res += "Source code:\n";
    for (auto &&fn : funs) {
      res += fn.second->toString() + "\n";
    }
    return res;
  }
};

} // namespace AST
