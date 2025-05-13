#include "parser.hpp"
#include "iparser.hpp"
#include <cassert>
#include <complex>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

using buffer = std::vector<Lexeme>;
using iter = buffer::const_iterator;
using spExpr = std::shared_ptr<Expr>;
using parseFn = std::function<spExpr(iter &, iter &)>;
using umap = std::unordered_map<std::string, parseFn>;
using tmap = std::unordered_map<std::string, TypeEntry>;
using precMap = std::unordered_map<LexemeType, size_t>;
using binOpMap = std::unordered_map<LexemeType, BinOp>;
using OptionTopLevel = std::optional<std::shared_ptr<TopLevel>>;
using OptionExpr = std::optional<std::shared_ptr<Expr>>;
using VarContext = std::unordered_map<std::string, std::shared_ptr<Var>>;

// TODO: make separate function for parsing types, arrays are cornercase i can't
// do the way im doing it right now.

struct Context {
  VarContext ctx;
};

static std::shared_ptr<Expr> expr(iter &start, iter &end, Context &ctx);
static OptionExpr maybeExpr(iter &start, iter &end, Context &ctx);

tmap types = {
    {"int", TypeEntry("int", TypeClass::Int)},
    {"bool", TypeEntry("bool", TypeClass::Bool)},
};

precMap precedences = {
    {LexemeType::Star, 10},
    {LexemeType::Div, 10},
    {LexemeType::Minus, 5},
    {LexemeType::Plus, 5},
};

binOpMap binOperators = {
    {LexemeType::Star, BinOp::Mul},
    {LexemeType::Div, BinOp::Div},
    {LexemeType::Minus, BinOp::Sub},
    {LexemeType::Plus, BinOp::Add},
};

static bool binOp(Lexeme &lexeme) {
  auto binops = {LexemeType::Star, LexemeType::Div, LexemeType::Minus,
                 LexemeType::Plus};
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

static void expectId(Lexeme &actual, std::string expected) {
  if (actual.type() != LexemeType::Id || actual.lexeme() != expected) {
    throw ParserException("expected id " + expected);
  }
}

static void expectLeftParen(Lexeme &lexeme) {
  expect(LexemeType::LParen, lexeme);
}

static void expectEOL(Lexeme &lexeme) { expect(LexemeType::EOL, lexeme); }

static void expectRightParen(Lexeme &lexeme) {
  expect(LexemeType::RParen, lexeme);
}

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
  if (start == end)
    throw ParserException("end is too soon");
}

static void dec(iter &start) { --start; }

static std::optional<std::shared_ptr<Var>> varDecl(iter &start, iter &end) {
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
  inc(start, end);

  auto ret = std::make_shared<Var>(varName.lexeme(), type);
  return ret;
}

static OptionTopLevel typeDecl(iter &start, iter &end) {
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
    auto var = varDecl(start, end);
    if (var.has_value()) {
      tDecl->fields.push_back(*var);
    } else {
      break;
    }
  }

  return tDecl;
}

static std::optional<std::shared_ptr<Var>> parseParam(iter &start, iter &end) {
  auto varName = *start;
  if (varName.type() != LexemeType::Id) {
    dec(start);
    return std::nullopt;
  }

  inc(start, end);
  auto tName = *start;
  expect(LexemeType::Id, tName);

  auto type = TypeEntry(tName.lexeme(), TypeClass::Custom);
  auto ret = std::make_shared<Var>(varName.lexeme(), type);
  return ret;
}

static OptionTopLevel functionDecl(iter &start, iter &end) {
  auto cur = *start;
  if (cur.type() != LexemeType::Fun) {
    return std::nullopt;
  }

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
    auto param = parseParam(start, end);
    if (param.has_value()) {
      params.push_back(*param);
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
    retType = TypeEntry(retTypeLexeme.lexeme(), TypeClass::Custom);
    inc(start, end);
    auto lbrace = *start;
    expect(LexemeType::LBrace, lbrace);
  } else if (!(retTypeLexeme.type() == LexemeType::LBrace)) {
    throw ParserException("expected type or left brace");
  }

  /*inc(start, end);*/
  // TODO: HERE SHOULD BE PARSING OF STATEMENTS

  inc(start, end);
  auto rbrace = *start;
  expect(LexemeType::RBrace, rbrace);

  auto fn = new Function(fnName.lexeme(), retType, params,
                         {}); // idk why it can't make_shared here
  auto ret = std::shared_ptr<Function>(fn);

  return ret;
}

std::shared_ptr<TopLevel> anyTopLevel(iter &start, iter &end) {
  auto topLevel = {typeDecl, functionDecl};
  for (auto &&fn : topLevel) {
    auto ret = fn(start, end);
    if (ret.has_value())
      return *ret;
  }
  throw ParserException("no top level constructions where expected one");
}

OptionTopLevel ParserImpl::parseTopLevel(std::vector<Lexeme> &lexems) {
  auto begin = lexems.cbegin();
  auto end = lexems.cend();
  auto tl = anyTopLevel(begin, end);
  return tl;
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

  std::vector<std::shared_ptr<Expr>> args;
  inc(start, end);
  while (argCheck) {
    auto arg = maybeExpr(start, end, ctx);
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

  auto ret = std::make_shared<Call>(fnName.lexeme(), args);
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

  auto type = TypeEntry("int", TypeClass::Int);
  auto ret = std::make_shared<Var>(tok.lexeme(), type);

  return ret;
}
static std::shared_ptr<Expr> binaryExpr(iter &start, iter &end, Context &ctx,
                                        std::shared_ptr<Expr> lhs,
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
    auto rhsOpt = maybeExpr(start, end, ctx);

    if (!rhsOpt.has_value()) {
      throw ParserException("expected right side of binary expression" +
                            lhs->toString() + " " + peekOp.lexeme());
    }

    auto rhs = *rhsOpt;
    auto nextOp = peek(start, end);
    if (!binOp(nextOp)) {
      return std::make_shared<BinExp>(lhs, rhs, op);
    }
    auto nextPrecedence = precedences[nextOp.type()];
    if (precedence < nextPrecedence) {
      rhs = binaryExpr(start, end, ctx, rhs, precedence + 1);
    }
    lhs = std::make_shared<BinExp>(lhs, rhs, op);
  }
}

static OptionExpr maybeExpr(iter &start, iter &end, Context &ctx) {
  auto exprs = {parseCall, parseId, parenExpr, parseNumber};
  for (auto &&fn : exprs) {
    auto ret = fn(start, end, ctx);
    if (ret.has_value())
      return *ret;
  }
  return std::nullopt;
}
static std::shared_ptr<Expr> expr(iter &start, iter &end, Context &ctx) {
  auto retOpt = maybeExpr(start, end, ctx);
  if (retOpt.has_value()) {
    auto ret = *retOpt;
    return binaryExpr(start, end, ctx, ret, 0);
  }

  throw ParserException("no expressions where expected one");
}

std::shared_ptr<Expr> ParserImpl::parse(std::vector<Lexeme> &lexems) {
  auto begin = lexems.cbegin();
  auto end = lexems.cend();
  Context ctx{};
  auto expression = expr(begin, end, ctx);
  return expression;
}
