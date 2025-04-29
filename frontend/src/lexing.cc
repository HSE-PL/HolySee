#include "lexing.h"

#include <algorithm>

using Kind = hsec::frontend::Token::Kind;

namespace hsec::frontend::lexing {

inline namespace {

bool isDigit(char c) { return '0' <= c && c <= '9'; }

bool isLetter(char c) {
  return c == '_' || ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

bool isSpace(char c) { return c == ' ' || c == '\t'; }

bool isWordChar(char c) { return isLetter(c) || isDigit(c); }

void advance(size_t& pos, std::string_view& view, size_t count) {
  pos += count;
  view = view.substr(count);
}

}  // namespace

Token Lexer::next(std::string_view view) {
  if (auto token = nextOnline(view); token)
    return token.value();
  if (indent) {
    --indent;
    return Token(Kind::end, pos);
  }
  return Token(Kind::eof, pos);
}

std::optional<Token> Lexer::nextOnline(std::string_view view) {
  Lexer& ctx = *this;
  char c;

  if (num_ends) {
    --num_ends;
    --indent;
    return Token(Kind::end, pos);
  }

  auto newline = false;
  size_t num_spaces;
  while (view.length()) {
    num_spaces = 0;
    while (num_spaces < view.length() && isSpace(c = view[num_spaces]))
      num_spaces++;
    if (num_spaces < view.length()) {
      if (view[num_spaces] == '\n')
        newline = true;
      else
        break;
      num_spaces++;
    }
    advance(pos, view, num_spaces);
  }

  if (view.empty())
    return std::nullopt;

  if (newline) {
    Indent indent;
    if (num_spaces) {
      if (ctx.indent)
        indent = ctx.indent.pattern();
      else {
        indent.style = (Indent::Style)(view[0]);
        indent.width = num_spaces;
      }
      size_t num_matching = 0;
      while (num_matching < num_spaces && view[num_matching] == (char)indent)
        num_matching++;
      num_matching -= num_matching % indent.width;
      indent.level = std::min(
          num_matching / indent.width,
          ctx.indent.level + (num_matching == num_spaces)
      );
      advance(pos, view, num_spaces);
      if (num_spaces -= indent.length())
        return Token(Kind::bad, pos - num_spaces, num_spaces);
      if (indent.level > ctx.indent.level) {
        ctx.indent = indent;
        return Token(Kind::begin, pos - indent.width, indent.width);
      }
    }
    if ((num_ends = ctx.indent - indent))
      return nextOnline(view);
    return Token(Kind::next, pos);
  } else
    advance(pos, view, num_spaces);

  Token token(Kind::bad, pos);
  if (isDigit(c)) {
    token.kind = Kind::integer;
    do {
      token.span.len++;
    } while (token.span.len < view.length() && isDigit(c = view[token.span.len])
    );
  } else if (isWordChar(c)) {
    token.kind = Kind::word;
    do {
      token.span.len++;
    } while (token.span.len < view.length()
             && isWordChar(c = view[token.span.len]));
  }
  advance(pos, view, token.span.len);
  return token;
}

}  // namespace hsec::frontend::lexing
