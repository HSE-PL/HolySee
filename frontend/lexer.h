#ifndef HSEC_FRONTEND_LEXING_H
#define HSEC_FRONTEND_LEXING_H

#include <optional>
#include <string_view>
#include <vector>

#include "token.h"

namespace hsec::frontend {

namespace lexing {

enum class Region : char {
  scope = '}',
  parens = ')',
  brackets = ']',
};

struct Indent {
  struct Style {
    char character;
    size_t width{};

    Style() = default;
    Style(char character, size_t width) : character(character), width(width) {}
  };

  Style style;
  size_t level{};

  Indent() : style{} {};
  Indent(Style style, size_t level = 0) : style(style), level(level) {}

  size_t length() const { return style.width * level; }

  explicit operator bool() const { return level; }

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

struct Context {
  size_t pos{};
  Indent indent;
  std::vector<Region> regions;
  bool skipNewline = true;
};

};  // namespace lexing

class Lexer : lexing::Context {
  size_t num_pending_scope_ends{};

 public:
  Token next(std::string_view);
  std::optional<Token> nextOnline(std::string_view);

  size_t getPos() const { return pos; }

  explicit operator bool() const { return regions.empty() && !indent; };

 private:
  std::optional<Token> pendingToken();
};

}  // namespace hsec::frontend

#endif
