#include "parsing.h"

#include <iostream>
#include <string_view>

#include "lexing.h"

using namespace hsec::frontend;

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

TestFile operator""_file(const char* file, size_t) { return TestFile(file); }

int main() {
  auto file =
      "type Smth struct\n"
      "\tx unit\n"
      "\ty unit\n"_file;
  std::cout << parsing::parseTypeDecl(file);
  std::cout << std::endl;
}
