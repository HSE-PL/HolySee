#ifndef HSEC_FRONTEND_LEXICON_H
#define HSEC_FRONTEND_LEXICON_H

#include <array>
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

  static const std::array<const Keyword, 11> values;

  static std::optional<Keyword> match(std::string_view sv) {
    for (const auto& kw : values) {
      if ((const char*)kw == sv)
        return kw;
    }
    return std::nullopt;
  }
};

using enum Keyword::Kind;

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
  size_t pos;
  size_t len;

  Token(Kind kind, size_t pos, size_t len) : kind(kind), pos(pos), len(len) {}
  Token(Kind kind, size_t pos) : Token(kind, pos, 0) {}
};

}  // namespace hsec::frontend

#endif
