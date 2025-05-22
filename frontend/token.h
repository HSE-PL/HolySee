#ifndef HSEC_FRONTEND_TOKEN_H
#define HSEC_FRONTEND_TOKEN_H

#include <cstddef>
#include <utility>

namespace hsec::frontend {

class Span {
  size_t pos;
  size_t len;

 public:
  Span(size_t pos) : Span(pos, 0) {}
  Span(size_t pos, size_t len) : pos(pos), len(len) {}

  size_t start() const { return pos; }
  size_t end() const { return pos + len; }
  size_t length() const { return len; }

  bool operator==(const Span& other) const = default;
};

struct Token {
  enum class Kind {
    // Auxilary tokens
    bad,
    eof,

    // Variable tokens
    ident,
    integer,

    // Scopes
    newline,
    scope_begin,
    scope_end,

    // Parens
    parens_begin,
    parens_end,

    // Brackets
    brackets_begin,
    brackets_end,

    // Punctuation
    plus,
    minus,
    asterisk,
    div_sign,
    equal_sign,
    colon,
    comma,
    dot,

    // Keywords
    kw_const,
    kw_else,
    kw_fun,
    kw_if,
    kw_inside,
    kw_loop,
    kw_return,
    kw_struct,
    kw_type,
    kw_union,
    kw_var,
  };

  Kind kind;
  Span span;

  template <typename... Args>
  Token(Kind kind, Args&&... args)
      : kind(kind), span(std::forward<Args>(args)...) {}

  operator Kind() const { return kind; }
  operator Span() const { return span; }
};

}  // namespace hsec::frontend

#endif
