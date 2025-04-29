#ifndef HSEC_FRONTEND_LEXING_H
#define HSEC_FRONTEND_LEXING_H

#include <optional>

#include "lexicon.h"

namespace hsec::frontend::lexing {

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
    level++;
    return *this;
  }

  Indent& operator--() {
    level++;
    return *this;
  }
};

class Lexer {
  size_t pos = 0;

  Indent indent;
  size_t num_ends = 0;

 public:
  Token next(std::string_view);
  std::optional<Token> nextOnline(std::string_view);

  size_t getPos() const { return pos; }

  explicit operator bool() { return !indent; };
};

}  // namespace hsec::frontend::lexing

#endif
