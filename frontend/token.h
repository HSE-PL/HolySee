#ifndef HSEC_FRONTEND_TOKEN_H
#define HSEC_FRONTEND_TOKEN_H

#include <cstddef>
#include <utility>

namespace hsec::frontend {

struct Span {
  size_t pos;
  size_t len;

  Span(size_t pos) : Span(pos, 0) {}
  Span(size_t pos, size_t len) : pos(pos), len(len) {}

  bool operator==(const Span& other) const = default;
};

struct Token {
  enum class Kind {
    bad,
    begin,
    end,
    eof,
    integer,
    ident,
    next,

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
