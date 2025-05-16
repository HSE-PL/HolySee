#include <algorithm>
#include <cctype>
#include <optional>
#include <sstream>

#include "gtest/gtest.h"
#include "lexer.h"

using ::testing::AssertionFailure;
using ::testing::AssertionResult;
using ::testing::AssertionSuccess;

using namespace hsec::frontend;

namespace {

const char* kindName(Token::Kind kind) {
  using enum Token::Kind;
  switch (kind) {
    case bad: return "bad";
    case begin: return "begin";
    case end: return "end";
    case eof: return "eof";
    case integer: return "integer";
    case ident: return "ident";
    case next: return "next";
    case kw_const: return "const";
    case kw_else: return "else";
    case kw_fun: return "fun";
    case kw_if: return "if";
    case kw_inside: return "inside";
    case kw_loop: return "loop";
    case kw_return: return "return";
    case kw_struct: return "struct";
    case kw_type: return "type";
    case kw_union: return "union";
    case kw_var: return "var"; break;
  };
}

class LexerTester {
  std::string_view src;
  Lexer lexer;
  size_t lastPos;

 public:
  LexerTester(std::string_view src) : src(src) {}

  AssertionResult nextTokenIs(Token expected) {
    if (auto actual = next(); actual != expected)
      return failure(actual);
    return AssertionSuccess();
  }

  AssertionResult nextTokenIs(Token::Kind kind) {
    if (auto token = next(); token.kind != kind)
      return failure(token);
    return AssertionSuccess();
  }

  AssertionResult nextTokenIs(Token::Kind kind, std::string_view value) {
    Span expected_span(src.find_first_of(value, lastPos), value.length());
    auto token = next();
    if (token.kind != kind || token.span != expected_span)
      return failure(token);
    return AssertionSuccess();
  }

  ~LexerTester() {
    EXPECT_TRUE(next().kind == Token::Kind::eof) << "lexer is not exhausted";
  }

 private:
  Token next() {
    auto token = lexer.next(src.substr(lexer.getPos()));
    lastPos = token.span.pos + token.span.len;
    return token;
  }

  std::string_view examine(Span span) { return src.substr(span.pos, span.len); }

  std::string repr(Token token) {
    auto value = examine(token);
    std::ostringstream repr;
    repr << "Token { kind: " << kindName(token) << ", ";
    auto isSpace = [](char c) -> bool {
      return c == ' ' || c == '\t' || c == '\n';
    };
    if (std::all_of(value.begin(), value.end(), isSpace))
      repr << "span: [" << token.span.pos << ", "
           << token.span.pos + token.span.len << ")";
    else
      repr << "value: \"" << value << "\"";
    repr << " }";
    return std::move(repr).str();
  }

  AssertionResult failure(Token token) {
    return AssertionFailure() << "got " << repr(token);
  }
};

}  // namespace

TEST(LexerTests, Empty) {
  LexerTester totallyEmpty("");
  LexerTester sematicallyEmpty(" \t\n\t\t\n\n");
}

TEST(LexerTests, Ident) {
  using enum Token::Kind;
  LexerTester tester("camelCase PascalCase snake_case CAPS withN0m8ers");
  ASSERT_TRUE(tester.nextTokenIs(ident, "camelCase"));
  ASSERT_TRUE(tester.nextTokenIs(ident, "PascalCase"));
  ASSERT_TRUE(tester.nextTokenIs(ident, "snake_case"));
  ASSERT_TRUE(tester.nextTokenIs(ident, "CAPS"));
  ASSERT_TRUE(tester.nextTokenIs(ident, "withN0m8ers"));
}

TEST(LexerTests, Numbers) {
  using enum Token::Kind;
  LexerTester tester("0 7 126 0042");
  ASSERT_TRUE(tester.nextTokenIs(integer, "0"));
  ASSERT_TRUE(tester.nextTokenIs(integer, "7"));
  ASSERT_TRUE(tester.nextTokenIs(integer, "126"));
  ASSERT_TRUE(tester.nextTokenIs(integer, "0042"));
}
