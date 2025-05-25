#include "parser.hpp"
#include "iparser.hpp"
#include <cassert>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

namespace AST {

using buffer = std::vector<Lexeme>;
using iter = buffer::const_iterator;
using spExpr = std::shared_ptr<AST::Expr>;
using parseFn = std::function<spExpr(iter &, iter &)>;
using umap = std::unordered_map<std::string, parseFn>;
using tmap = std::unordered_map<std::string, TypeEntry>;
using precMap = std::unordered_map<LexemeType, size_t>;
using binOpMap = std::unordered_map<LexemeType, BinOp>;
using OptionTopLevel = std::optional<std::shared_ptr<AST::TopLevel>>;
using OptionExpr = std::optional<std::shared_ptr<AST::Expr>>;
using OptionStmt = std::optional<std::shared_ptr<AST::Stmt>>;
using OptionVar = std::optional<std::shared_ptr<AST::Var>>;
using VarContext = std::unordered_map<std::string, std::shared_ptr<AST::Var>>;

// TODO: make separate function for parsing types, arrays are cornercase i can't
// do the way im doing it right now.

struct Context {
  VarContext ctx;
  tmap &typeCtx; // very questionable reference. but as of now i always know
                 // that this reference will outlive every Context.
                 // if it starts exploding look this way.
  Context(tmap &typeCtx) : typeCtx(typeCtx) {}
  void addVar(std::string name, std::shared_ptr<AST::Var> var) {
    ctx.insert({name, var});
  }

  void addType(std::string name, TypeEntry var) { typeCtx.insert({name, var}); }
  OptionVar lookup(std::string name) {
    if (ctx.contains(name)) {
      return ctx[name];
    }
    return std::nullopt;
  }
};

static std::shared_ptr<AST::Expr> expr(iter &start, iter &end, Context &ctx);
static OptionExpr singleExpr(iter &start, iter &end, Context &ctx);
static OptionStmt stmt(iter &start, iter &end, Context &ctx);

precMap precedences = {
    {LexemeType::Star, 10},        {LexemeType::Div, 10},
    {LexemeType::Minus, 5},        {LexemeType::Plus, 5},
    {LexemeType::Or, 3},           {LexemeType::And, 3},
    {LexemeType::Equality, 3},     {LexemeType::Greater, 3},
    {LexemeType::GreaterEqual, 3}, {LexemeType::Less, 3},
    {LexemeType::LessEqual, 3},    {LexemeType::NotEqual, 3},
};

binOpMap binOperators = {
    {LexemeType::Star, BinOp::Mul},
    {LexemeType::And, BinOp::And},
    {LexemeType::Or, BinOp::Or},
    {LexemeType::Div, BinOp::Div},
    {LexemeType::Minus, BinOp::Sub},
    {LexemeType::Plus, BinOp::Add},
    {LexemeType::Equality, BinOp::Equals},
    {LexemeType::Less, BinOp::Less},
    {LexemeType::LessEqual, BinOp::LessEqual},
    {LexemeType::GreaterEqual, BinOp::GreaterEqual},
    {LexemeType::Greater, BinOp::Greater},
    {LexemeType::NotEqual, BinOp::NotEqual},
};

static bool binOp(Lexeme &lexeme) {
  auto binops = {
      LexemeType::Star,      LexemeType::Div,     LexemeType::Minus,
      LexemeType::Plus,      LexemeType::And,     LexemeType::Or,
      LexemeType::Equality,  LexemeType::Greater, LexemeType::GreaterEqual,
      LexemeType::LessEqual, LexemeType::Less,    LexemeType::NotEqual};
  for (auto binop : binops) {
    if (lexeme.type() == binop)
      return true;
  }
  return false;
}

static void expect(LexemeType expected, Lexeme &actual) {
  if (expected != actual.type())
    throw ParserException("expect failed, expected" +
                          std::to_string((int)expected) +
                          ", actual: " + actual.lexeme());
}

static void expectLeftParen(Lexeme &lexeme) {
  expect(LexemeType::LParen, lexeme);
}

static void expectEOL(Lexeme &lexeme) { expect(LexemeType::EOL, lexeme); }

static Lexeme peek(iter &start, iter &end) {
  auto peekachu = ++start;
  if (peekachu == end) {
    throw ParserException("tried to peek and it wasn't fortunate");
  }
  auto peeked = *peekachu;
  --start;
  return peeked;
}

static void inc(iter &start, iter &end) {
  ++start;
  if (start == end) {
    throw ParserException("end is too soon");
  }
}

static void dec(iter &start) { --start; }

static std::optional<std::shared_ptr<Var>> fieldDecl(iter &start, iter &end,
                                                     tmap &types) {
  auto varName = *start;
  if (varName.type() != LexemeType::Id) {
    return std::nullopt;
  }

  inc(start, end);

  auto typeName = *start;

  if (typeName.type() != LexemeType::Id) {
    dec(start);
    return std::nullopt;
  }

  if (!types.contains(typeName.lexeme())) {
    throw ParserException("there is no type by name " + typeName.lexeme());
  }

  auto type = types[typeName.lexeme()];

  inc(start, end);
  auto cur = *start;

  expectEOL(cur);

  auto ret = std::make_shared<Var>(varName.lexeme(), type);
  return ret;
}

static OptionTopLevel typeDecl(iter &start, iter &end, tmap &types) {
  auto cur = *start;
  if (cur.type() != LexemeType::Type) {
    return std::nullopt;
  }
  inc(start, end);

  auto typeName = *start;
  expect(LexemeType::Id, typeName);

  types.insert(
      {typeName.lexeme(), TypeEntry(typeName.lexeme(), TypeClass::Custom)});

  inc(start, end);

  cur = *start;
  expect(LexemeType::Struct, cur);
  inc(start, end);

  cur = *start;
  expect(LexemeType::LBrace, cur);
  inc(start, end);

  auto tDecl = std::make_shared<TypeDeclaration>(typeName.lexeme());
  while (true) {
    auto var = fieldDecl(start, end, types);
    if (var.has_value()) {
      tDecl->fields.push_back(*var);
      inc(start, end);
    } else {
      break;
    }
  }

  auto rbrace = *start;
  expect(LexemeType::RBrace, rbrace);

  return tDecl;
}

static std::optional<std::shared_ptr<Var>> parseParam(iter &start, iter &end,
                                                      tmap &types) {
  auto varName = *start;
  if (varName.type() != LexemeType::Id) {
    dec(start);
    return std::nullopt;
  }

  inc(start, end);
  auto tName = *start;
  expect(LexemeType::Id, tName);

  auto type = types[tName.lexeme()];
  auto ret = std::make_shared<Var>(varName.lexeme(), type);
  return ret;
}

static OptionTopLevel functionDecl(iter &start, iter &end, tmap &types) {
  auto cur = *start;
  if (cur.type() != LexemeType::Fun) {
    return std::nullopt;
  }
  Context ctx{types};

  inc(start, end);

  auto fnName = *start;
  expect(LexemeType::Id, fnName);

  inc(start, end);
  auto lparen = *start;
  expectLeftParen(lparen);

  std::vector<std::shared_ptr<Var>> params{};

  auto peeked = peek(start, end);
  bool paramCheck = true;
  if (peeked.type() == LexemeType::RParen)
    paramCheck = false;

  inc(start, end);
  while (paramCheck) {
    auto param = parseParam(start, end, types);
    if (param.has_value()) {
      auto variable = *param;
      params.push_back(variable);
      ctx.addVar(variable->id, variable);
    } else {
      throw ParserException("expected parameter got" + start->lexeme());
    }

    inc(start, end);
    auto delim = *start;
    if (delim.type() == LexemeType::RParen)
      break;
    if (delim.type() == LexemeType::Comma) {
      inc(start, end);
      continue;
    }
    throw ParserException("incorrect delimiter in params " + delim.lexeme());
  }

  inc(start, end);
  auto retTypeLexeme = *start;
  TypeEntry retType{"void", TypeClass::Void};
  if (retTypeLexeme.type() == LexemeType::Id) {
    retType = types[retTypeLexeme.lexeme()];
    inc(start, end);
    auto lbrace = *start;
    expect(LexemeType::LBrace, lbrace);
  } else if (!(retTypeLexeme.type() == LexemeType::LBrace)) {
    throw ParserException("expected type or left brace");
  }

  std::vector<std::shared_ptr<Stmt>> stmts;
  inc(start, end);
  while (true) {
    auto statement = stmt(start, end, ctx);
    if (!statement.has_value()) {
      break;
    }
    stmts.push_back(*statement);
    inc(start, end);
  }

  auto rbrace = *start;
  expect(LexemeType::RBrace, rbrace);

  auto fn =
      std::make_shared<Function>(fnName.lexeme(), retType, params,
                                 stmts); // idk why it can't make_shared here
  /*auto ret = std::shared_ptr<Function>(fn);*/

  return fn;
}

std::shared_ptr<TopLevel> anyTopLevel(iter &start, iter &end, tmap &types) {
  auto topLevel = {typeDecl, functionDecl};
  for (auto &&fn : topLevel) {
    auto ret = fn(start, end, types);
    if (ret.has_value()) {
      ++start;
      return *ret;
    }
  }
  throw ParserException("no top level constructions where expected one");
}

OptionTopLevel parseTopLevel(iter &start, iter &end, tmap &types) {
  return anyTopLevel(start, end, types);
}

static OptionExpr parenExpr(iter &start, iter &end, Context &ctx) {
  auto tok = *start;
  if (tok.type() != LexemeType::LParen)
    return std::nullopt;
  inc(start, end);
  auto expressionParsed = expr(start, end, ctx);

  inc(start, end);
  tok = *start;
  expect(LexemeType::RParen, tok);
  return expressionParsed;
  //
}

static OptionExpr parseNumber(iter &start, iter &end, Context &ctx) {
  auto tok = *start;
  if (tok.type() != LexemeType::Int)
    return std::nullopt;
  auto ret = std::make_shared<Const>(std::stoi(tok.lexeme()));
  return ret;
}

static OptionExpr parseCall(iter &start, iter &end, Context &ctx) {
  auto fnName = *start;
  if (fnName.type() != LexemeType::Id)
    return std::nullopt;
  inc(start, end);
  auto tok = *start;
  if (tok.type() != LexemeType::LParen) {
    dec(start);
    return std::nullopt;
  }

  bool argCheck = true;
  auto peeked = peek(start, end);
  if (peeked.type() == LexemeType::RParen)
    argCheck = false;

  std::vector<std::shared_ptr<AST::Expr>> args;
  inc(start, end);
  while (argCheck) {
    auto arg = singleExpr(start, end, ctx);
    if (arg.has_value()) {
      args.push_back(*arg);
    } else {
      throw ParserException("expected parameters got" + start->lexeme());
    }
    inc(start, end);
    auto delim = *start;
    if (delim.type() == LexemeType::RParen)
      break;
    if (delim.type() == LexemeType::Comma) {
      inc(start, end);
      continue;
    }
  }

  auto ret = std::make_shared<AST::Call>(fnName.lexeme(), args);
  return ret;
}

static OptionExpr parseId(iter &start, iter &end, Context &ctx) {
  auto tok = *start;
  if (tok.type() != LexemeType::Id)
    return std::nullopt;
  auto peeked = peek(start, end);
  if (peeked.type() == LexemeType::LParen) {
    return parseCall(start, end, ctx);
  }
  // TODO: here we should check members(fields or methods)
  // TODO: here we should check VarContext
  auto var = ctx.lookup(tok.lexeme());
  if (!var.has_value()) {
    throw ParserException("use of undeclared variable " + tok.lexeme());
  }
  /*auto type = TypeEntry("int", TypeClass::Int);*/
  /*auto ret = std::make_shared<Var>(tok.lexeme(), type);*/

  return *var;
}
static std::shared_ptr<AST::Expr> binaryExpr(iter &start, iter &end,
                                             Context &ctx,
                                             std::shared_ptr<AST::Expr> lhs,
                                             size_t prec) {

  while (true) {
    auto peekOp = peek(start, end);
    if (!binOp(peekOp))
      return lhs;

    auto precedence = precedences[peekOp.type()];

    if (precedence < prec)
      return lhs;

    auto op = binOperators[peekOp.type()];
    inc(start, end);

    inc(start, end);
    auto rhsOpt = singleExpr(start, end, ctx);

    if (!rhsOpt.has_value()) {
      throw ParserException("expected right side of binary expression" +
                            lhs->toString() + " " + peekOp.lexeme());
    }

    auto rhs = *rhsOpt;
    auto nextOp = peek(start, end);
    if (!binOp(nextOp)) {
      return std::make_shared<AST::BinExp>(lhs, rhs, op);
    }
    auto nextPrecedence = precedences[nextOp.type()];
    if (precedence < nextPrecedence) {
      rhs = binaryExpr(start, end, ctx, rhs, precedence + 1);
    }
    lhs = std::make_shared<AST::BinExp>(lhs, rhs, op);
  }
}

static OptionExpr singleExpr(iter &start, iter &end, Context &ctx) {
  auto exprs = {parseCall, parseId, parenExpr, parseNumber};
  for (auto &&fn : exprs) {
    auto ret = fn(start, end, ctx);
    if (ret.has_value())
      return *ret;
  }
  return std::nullopt;
}

static OptionExpr stmtExpr(iter &start, iter &end, Context &ctx) {
  auto retOpt = singleExpr(start, end, ctx);
  if (retOpt.has_value()) {
    auto ret = *retOpt;
    auto totalRet = binaryExpr(start, end, ctx, ret, 0);
    inc(start, end);
    auto eol = *start;
    expectEOL(eol);
    return totalRet;
  }

  return std::nullopt;
}

static std::shared_ptr<AST::Expr> expr(iter &start, iter &end, Context &ctx) {
  auto retOpt = singleExpr(start, end, ctx);
  if (retOpt.has_value()) {
    auto ret = *retOpt;
    return binaryExpr(start, end, ctx, ret, 0);
  }

  throw ParserException("no expressions where expected one");
}

static OptionStmt varDecl(iter &start, iter &end, Context &ctx) {
  auto var = *start;
  if (var.type() != LexemeType::Var) {
    return std::nullopt;
  }

  inc(start, end);
  auto typeLexeme = *start;
  expect(LexemeType::Id, typeLexeme);

  if (!ctx.typeCtx.contains(typeLexeme.lexeme())) {
    throw ParserException("type of var is not defined, found " +
                          typeLexeme.lexeme());
  }

  auto type = ctx.typeCtx.at(typeLexeme.lexeme());
  std::vector<std::shared_ptr<AST::Var>> vars;

  while (true) {
    inc(start, end);
    auto varName = *start;
    expect(LexemeType::Id, varName);

    auto var = std::make_shared<AST::Var>(varName.lexeme(), type);
    vars.push_back(var);
    ctx.addVar(varName.lexeme(), var);

    inc(start, end);
    auto delim = *start;
    if (delim.type() == LexemeType::EOL) {
      auto vardecl = std::make_shared<AST::VarDecl>(vars);
      return vardecl;
    }
    if (delim.type() != LexemeType::Comma) {
      throw ParserException("unexpected token. waited for EOL or Comma, got " +
                            delim.lexeme());
    }
  }
}

static OptionStmt returnStmt(iter &start, iter &end, Context &ctx) {
  auto ret = *start;
  if (ret.type() != LexemeType::Return) {
    return std::nullopt;
  }

  inc(start, end);
  auto expression = stmtExpr(start, end, ctx);
  if (expression.has_value()) {
    return std::make_shared<AST::Ret>(*expression);
  }
  auto eol = *start;
  expectEOL(eol);

  return std::make_shared<AST::Ret>();
}

static OptionStmt assign(iter &start, iter &end, Context &ctx) {
  auto varName = *start;
  if (varName.type() != LexemeType::Id) {
    return std::nullopt;
  }

  auto equals = peek(start, end);

  if (equals.type() != LexemeType::Equals) {
    return std::nullopt;
  }

  inc(start, end);
  inc(start, end);
  auto expression = expr(start, end, ctx);

  inc(start, end);
  auto eol = *start;
  expectEOL(eol);

  auto var = ctx.lookup(varName.lexeme());
  if (!var.has_value()) {
    throw ParserException("tried to assign to non-declared variable " +
                          varName.lexeme());
  }

  auto ret = std::make_shared<AST::Assign>(*var, expression);

  return ret;
}

static OptionStmt stmt(iter &start, iter &end, Context &ctx) {
  auto exprs = {varDecl, assign, returnStmt};
  for (auto &&fn : exprs) {
    auto ret = fn(start, end, ctx);
    if (ret.has_value())
      return *ret;
  }
  return stmtExpr(start, end, ctx);
}

TranslationUnit ParserImpl::parse(std::vector<Lexeme> &lexemes) {
  auto begin = lexemes.cbegin();
  auto end = lexemes.cend();
  TranslationUnit program{};
  while (begin != end) {
    auto topLevelOpt = parseTopLevel(begin, end, program.types);
    // very questionable break.
    // don't care as of now tho.
    if (!topLevelOpt.has_value()) {
      break;
    }
    auto topLevel = *topLevelOpt;
    auto tryFn = std::dynamic_pointer_cast<AST::Function>(topLevel);
    if (tryFn) {
      program.addFn(tryFn->id, tryFn);
      continue;
    }
    auto tryType = std::dynamic_pointer_cast<AST::TypeDeclaration>(topLevel);
    if (tryType) {
      program.addTypeDecl(tryType->type, tryType);
      continue;
    }
    throw ParserException("not typedecl or function but some other toplevel");
  }
  return program;
}
} // namespace AST
