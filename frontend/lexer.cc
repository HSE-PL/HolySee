#include "lexer.h"

using enum hsec::frontend::lexing::Region;
using enum hsec::frontend::Token::Kind;
using namespace hsec::frontend::lexing;

namespace hsec::frontend {

inline namespace {

bool isDigit(char c) { return '0' <= c && c <= '9'; }

bool isLetter(char c) {
  return c == '_' || ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

bool isSpace(char c) { return c == ' ' || c == '\t' || c == '\n'; }

bool isWordChar(char c) { return isLetter(c) || isDigit(c); }

std::optional<Token::Kind> matchKeyword(std::string_view word) {
  if (word == "const")
    return kw_const;
  if (word == "else")
    return kw_else;
  if (word == "fun")
    return kw_fun;
  if (word == "if")
    return kw_if;
  if (word == "inside")
    return kw_inside;
  if (word == "loop")
    return kw_loop;
  if (word == "return")
    return kw_return;
  if (word == "struct")
    return kw_struct;
  if (word == "type")
    return kw_type;
  if (word == "union")
    return kw_union;
  if (word == "var")
    return kw_var;
  return std::nullopt;
}

Indent scanIndent(
    std::string_view view, std::optional<Indent::Style> style_, size_t max_level
) {
  if (view.empty())
    return Indent();
  Indent::Style style = style_.value_or(Indent::Style(view[0], view.length()));
  auto num_matching = view.find_first_not_of(style.character);
  if (num_matching == view.npos)
    num_matching = view.length();
  return Indent(style, std::min(num_matching / style.width, max_level));
}

#define MATCHER \
  (const Context& ctx, std::string_view view)->std::optional<Token>

consteval auto fixedMatcher(Token::Kind kind, std::string_view value) {
  return [=] MATCHER {
    if (view.starts_with(value))
      return Token(kind, ctx.pos, value.length());
    return {};
  };
}

constexpr auto matchers = std::make_tuple(
    [] MATCHER {
      if (!isDigit(view[0]))
        return std::nullopt;
      size_t len{};
      while (len < view.length() && isDigit(view[len]))
        len++;
      return Token(integer, ctx.pos, len);
    },
    [] MATCHER {
      if (!isLetter(view[0]))
        return std::nullopt;
      size_t len{};
      while (len < view.length() && isWordChar(view[len]))
        len++;
      return Token(
          matchKeyword(view.substr(0, len)).value_or(ident), ctx.pos, len
      );
    },
    [] MATCHER {
      if (ctx.regions.empty())
        return std::nullopt;
      if (view[0] != (char)ctx.regions.back())
        return std::nullopt;
      Token::Kind kind;
      switch (ctx.regions.back()) {
        case Region::scope: kind = scope_end; break;
        case Region::parens: kind = parens_end; break;
        case Region::brackets: kind = brackets_end; break;
        default: return std::nullopt;
      }
      return Token(kind, ctx.pos, 1);
    },
    fixedMatcher(scope_begin, "{"),
    fixedMatcher(parens_begin, "("),
    fixedMatcher(brackets_begin, "["),
    fixedMatcher(newline, ";"),
    fixedMatcher(plus, "+"),
    fixedMatcher(minus, "-"),
    fixedMatcher(asterisk, "*"),
    fixedMatcher(div_sign, "/"),
    fixedMatcher(equal_sign, "="),
    fixedMatcher(colon, ":"),
    fixedMatcher(comma, ","),
    fixedMatcher(dot, ".")
);

#undef MATCHER

template <size_t I = 0>
inline Token match(const Context& ctx, std::string_view view) {
  if constexpr (I == std::tuple_size_v<decltype(matchers)>)
    return Token(bad, ctx.pos, 1);
  else {
    auto token = std::get<I>(matchers)(ctx, view);
    if (token)
      return token.value();
    return match<I + 1>(ctx, view);
  }
}

}  // namespace

std::optional<Token> Lexer::pendingToken() {
  if (num_pending_scope_ends) {
    num_pending_scope_ends--;
    return Token(scope_end, pos - indent.length(), indent.length());
  }
  return std::nullopt;
}

Token Lexer::next(std::string_view view) {
  if (auto token = nextOnline(view); token)
    return token.value();
  if (indent) {
    --indent;
    return Token(scope_end, pos);
  }
  return Token(eof, pos);
}

std::optional<Token> Lexer::nextOnline(std::string_view view) {
  char c;

  if (auto token = pendingToken(); token)
    return token;

  bool indent_enabled = regions.empty();
  size_t num_spaces{};
  size_t line_start = pos ? view.npos : 0;
  while (num_spaces < view.length() && isSpace(c = view[num_spaces])) {
    num_spaces++;
    if (indent_enabled && c == '\n')
      line_start = num_spaces;
  }

  if (num_spaces == view.length()) {
    pos += num_spaces;
    return std::nullopt;
  }

  std::optional<Token> indent_token;
  if (line_start != view.npos && indent_enabled) {
    pos += line_start;
    num_spaces -= line_start;
    view.remove_prefix(line_start);

    auto new_indent = scanIndent(
        view.substr(0, num_spaces),
        indent ? std::optional(indent.style) : std::nullopt,
        indent.level + 1
    );
    if (size_t delta; (delta = num_spaces - new_indent.length()))
      indent_token = Token(bad, line_start + new_indent.length(), delta);
    else {
      size_t indent_delta = new_indent - indent;
      indent = new_indent;
      switch (indent_delta) {
        case 1:
          indent_token = Token(scope_begin, pos, new_indent.length());
          break;
        case 0:
          if (!skipNewline)
            indent_token = Token(newline, pos, new_indent.length());
          break;
        default:
          num_pending_scope_ends = -indent_delta;
          indent_token = pendingToken();
          break;
      }
    }
  }
  pos += num_spaces;
  view.remove_prefix(num_spaces);
  if (indent_token)
    return indent_token;

  Token token = match(*this, view);
  skipNewline = false;
  switch (token.kind) {
    case Token::Kind::newline: [[fallthrough]];
    case Token::Kind::comma: skipNewline = true; break;

    case Token::Kind::scope_begin: regions.push_back(scope); break;
    case Token::Kind::parens_begin: regions.push_back(parens); break;
    case Token::Kind::brackets_begin: regions.push_back(brackets); break;

    case Token::Kind::scope_end: [[fallthrough]];
    case Token::Kind::parens_end: [[fallthrough]];
    case Token::Kind::brackets_end: regions.pop_back(); break;
    default:
  }
  pos = token.span.end();
  return token;
}

}  // namespace hsec::frontend
