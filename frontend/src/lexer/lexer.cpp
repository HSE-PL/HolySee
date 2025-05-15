#include "lexer.hpp"
#include "ilexer.hpp"
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

using view = std::string_view;
using buffer = std::vector<Lexeme>;
using iter = view::const_iterator;
using kmap = std::unordered_map<std::string, LexemeType>;
using lexemeMap = std::unordered_map<LexemeType, std::string>;

kmap keywordsLexeme = {
    {"type", LexemeType::Type},   {"struct", LexemeType::Struct},
    {"fun", LexemeType::Fun},     {"true", LexemeType::True},
    {"false", LexemeType::False}, {"var", LexemeType::Var},
    {"ret", LexemeType::Return},
};

lexemeMap lexemes = {
    {LexemeType::Type, "type"},     {LexemeType::Fun, "fun"},
    {LexemeType::Struct, "struct"}, {LexemeType::Equals, "="},
    {LexemeType::Id, "id"},         {LexemeType::Int, "int"},
    {LexemeType::LParen, "LParen"}, {LexemeType::RParen, "RParen"},
    {LexemeType::RBrace, "RBrace"}, {LexemeType::LBrace, "LBrace"},
    {LexemeType::EOL, "EOL"},       {LexemeType::Comma, "Comma"},
    {LexemeType::True, "true"},     {LexemeType::False, "false"},
    {LexemeType::Dot, "dot"},       {LexemeType::Plus, "Plus"},
    {LexemeType::Minus, "Minus"},   {LexemeType::Star, "*"},
    {LexemeType::Div, "/"},         {LexemeType::Var, "Var"},
    {LexemeType::Return, "Return"},
};

static void skipWhitespace(iter &input, iter &end) {
  while ((input != end) && isspace(*input))
    ++input;
}

std::string Lexeme::toString() {
  auto lexeme = lexemes.at(this->type());
  auto str = this->lexeme_;
  return lexeme + " " + str;
}

// we assume that input is in position where first symbol is, in fact, number.
static Lexeme lexInt(iter &input, iter &inputEnd) {
  std::string num;
  for (; input != inputEnd; ++input) {
    auto ch = *input;
    if (!isdigit(ch))
      break;
    num.push_back(ch);
  }
  return Lexeme(LexemeType::Int, num);
}

static Lexeme lexId(iter &input, iter &inputEnd) {
  std::string id;
  for (; input != inputEnd; ++input) {
    auto ch = *input;
    if ((!isalnum(ch)) && (ch != '_') && (ch != '-'))
      break;
    id.push_back(ch);
  }
  if (keywordsLexeme.contains(id)) {
    return Lexeme(keywordsLexeme[id], id);
  }
  return Lexeme(LexemeType::Id, id);
}

// it takes input and returns iter to the first element
// after the lexeme
static Lexeme lex(iter &input, iter &inputEnd) {
  auto ch = *input;
  switch (ch) {
  case '(': {
    auto lexemeStr = std::string{ch};
    ++input;
    return Lexeme(LexemeType::LParen, lexemeStr);
  } break;
  case ',': {
    auto lexemeStr = std::string{ch};
    ++input;
    return Lexeme(LexemeType::Comma, lexemeStr);
  } break;
  case '*': {
    auto lexemeStr = std::string{ch};
    ++input;
    return Lexeme(LexemeType::Star, lexemeStr);
  } break;
  case '-': {
    auto lexemeStr = std::string{ch};
    ++input;
    return Lexeme(LexemeType::Minus, lexemeStr);
  } break;
  case '+': {
    auto lexemeStr = std::string{ch};
    ++input;
    return Lexeme(LexemeType::Plus, lexemeStr);
  } break;
  case '/': {
    auto lexemeStr = std::string{ch};
    ++input;
    return Lexeme(LexemeType::Div, lexemeStr);
  } break;
  case '.': {
    auto lexemeStr = std::string{ch};
    ++input;
    return Lexeme(LexemeType::Dot, lexemeStr);
  } break;
  case '{': {
    auto lexemeStr = std::string{ch};
    ++input;
    return Lexeme(LexemeType::LBrace, lexemeStr);
  } break;
  case '}': {
    auto lexemeStr = std::string{ch};
    ++input;
    return Lexeme(LexemeType::RBrace, lexemeStr);
  } break;
  case ';': {
    auto lexemeStr = std::string{ch};
    ++input;
    return Lexeme(LexemeType::EOL, lexemeStr);
  } break;
  case '=': {
    auto lexemeStr = std::string{ch};
    ++input;
    return Lexeme(LexemeType::Equals, lexemeStr);
  } break;

  case ')': {
    auto lexemeStr = std::string{ch};
    ++input;
    return Lexeme(LexemeType::RParen, lexemeStr);
  } break;
  default:
    if (isdigit(ch) || ch == '-') {
      return lexInt(input, inputEnd);
    }
    if (isalpha(ch)) {
      return lexId(input, inputEnd);
    }
    throw LexerException("Symbol is not lexable, gg " + std::to_string(ch));
  }
}

static void lexStr(iter &input, iter &end, buffer &buffer) {
  skipWhitespace(input, end);
  for (; input != end;) {
    buffer.push_back(lex(input, end));
    skipWhitespace(input, end);
  }
}

std::vector<Lexeme> LexerImpl::lex(const view stringInput) {
  iter iterator = stringInput.cbegin();
  iter iend = stringInput.cend();
  std::vector<Lexeme> retVec{};
  lexStr(iterator, iend, retVec);
  return retVec;
}
