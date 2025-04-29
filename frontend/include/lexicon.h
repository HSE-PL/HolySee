#ifndef HSEC_FRONTEND_LEXICON_H
#define HSEC_FRONTEND_LEXICON_H

#include <array>
#include <concepts>
#include <cstddef>
#include <optional>
#include <string_view>

namespace hsec::frontend {

struct Keyword {
  enum Kind {
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

  Keyword(Kind kind) : kind(kind) {}
  operator Kind() const { return kind; }
  operator int() const { return kind; }
  operator const char*() const;

  bool operator==(Keyword other) const { return kind == other.kind; }

  static const std::array<const Keyword, 13> values;

  static std::optional<Keyword> match(std::string_view sv) {
    for (const auto& kw : values) {
      if ((const char*)kw == sv)
        return kw;
    }
    return std::nullopt;
  }
};

using enum Keyword::Kind;

struct Span {
  size_t pos;
  size_t len;

  Span(size_t pos) : Span(pos, 0) {}
  Span(size_t pos, size_t len) : pos(pos), len(len) {}

  size_t begin() const { return pos; }
  size_t end() const { return pos + len; }

  Span operator+(const Span& other) const {
    return Span(pos, other.pos - len + other.len);
  };
};

struct Token {
  enum class Kind {
    bad,
    begin,
    end,
    eof,
    integer,
    next,
    word,
  };

  Kind kind;
  Span span;

  template <typename... Args>
  Token(Kind kind, Args&&... args)
      : kind(kind), span(std::forward<Args>(args)...) {}

  operator Span() const { return span; }
};

}  // namespace hsec::frontend

#endif
