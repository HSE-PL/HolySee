#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

enum class LexemeType {
  LParen,
  LBrace,
  RBrace,
  RParen,
  Dot,
  EOL,
  Id,
  Int,
  Equals,
  Equality,
  Type,
  Struct,
  Fun,
  Comma,
  True,
  False,
  Plus,
  Minus,
  Div,
  Star,
  Var,
  Return,
  And,
  Or,
  Less,
  LessEqual,
  Greater,
  GreaterEqual,
  NotEqual,
  If,
  While,
  Else,
};

class LexerException {
public:
  std::string error_msg;
  LexerException(std::string error) : error_msg(error) {}
};

// could do some spans. couldn't care tho.
class Lexeme {
  LexemeType type_;
  std::string lexeme_;

public:
  Lexeme(LexemeType type, std::string &lexeme) : type_(type), lexeme_(lexeme) {}
  LexemeType type() const { return type_; }
  const std::string lexeme() const { return lexeme_; }
  std::string toString();
};

class ILexer {

public:
  virtual std::vector<Lexeme> lex(const std::string_view stringInput) = 0;
  virtual ~ILexer() {};
};
