#ifndef HSEC_FRONTEND_LEXING_H
#define HSEC_FRONTEND_LEXING_H

#include <optional>
#include <string_view>

#include "token.h"

namespace hsec::frontend {

namespace lexing {

enum class Region : char {
  Parens = '(',
  Brackets = '[',
  Braces = '{',
};

struct Indent {
  enum class Style : char {
    spaces = ' ',
    tabs = '\t',
  } style;
  size_t width = 0;
  size_t level = 0;

  size_t length() const { return width * level; }

  explicit operator bool() const { return level; }
  explicit operator char() const { return (char)style; }

  Indent pattern() const {
    auto pattern = *this;
    pattern.level = 0;
    return pattern;
  }

  ptrdiff_t operator-(const Indent& other) const {
    return (ptrdiff_t)level - other.level;
  }

  Indent& operator++() {
    ++level;
    return *this;
  }

  Indent& operator--() {
    --level;
    return *this;
  }
};

};  // namespace lexing

class Lexer {
  size_t pos = 0;

  lexing::Indent indent;
  size_t num_ends = 0;

 public:
  Token next(std::string_view);
  std::optional<Token> nextOnline(std::string_view);

  size_t getPos() const { return pos; }

  explicit operator bool() const { return !indent; };

 private:
  size_t advance(size_t count) {
    pos += count;
    return count;
  };
};

}  // namespace hsec::frontend

#endif
