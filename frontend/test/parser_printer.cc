#include <iostream>
#include <sstream>
#include <string_view>

#include "ast.h"
#include "ast/print.h"
#include "gtest/gtest.h"
#include "lexing.h"
#include "parsing.h"

using namespace hsec::frontend;
using lexing::Indent;

struct TestFile {
  lexing::Lexer ctx;
  std::string_view view_;
  std::optional<Token> buf;

  TestFile(std::string_view view) : view_(view) {};

  std::string_view view(Span span) { return view_.substr(span.pos, span.len); }

  Token next() {
    if (buf)
      return std::exchange(buf, std::nullopt).value();
    return ctx.next(view_.substr(ctx.getPos()));
  }

  const Token& peek() {
    if (!buf)
      buf = next();
    return *buf;
  }
};

std::string parse(std::string_view file) {
  TestFile stream(file);
  std::stringstream out;
  ast::print::OStreamPrinter printer(
      out, Indent{.style = Indent::Style::spaces, .width = 4}
  );
  for (const auto& decl : parsing::parse(stream))
    printer << *decl;
  return std::move(out).str();
}

int main() {
  std::ostringstream input;
  input << std::cin.rdbuf();
  std::cout << parse(input.view()) << std::flush;
}
