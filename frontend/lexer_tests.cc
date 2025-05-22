#include <algorithm>
#include <sstream>

#include "gtest/gtest.h"
#include "lexer.h"

using ::testing::AssertionFailure;
using ::testing::AssertionResult;
using ::testing::AssertionSuccess;

using namespace hsec::frontend;
using enum Token::Kind;

namespace {

const char* kindName(Token::Kind kind) {
  using enum Token::Kind;
  switch (kind) {
    case bad: return "bad";
    case eof: return "eof";
    case ident: return "ident";
    case integer: return "integer";
    case newline: return "newline";
    case scope_begin: return "scope_begin";
    case scope_end: return "scope_end";
    case parens_begin: return "parens_begin";
    case parens_end: return "parens_end";
    case brackets_begin: return "brackets_begin";
    case brackets_end: return "brackets_end";
    case plus: return "plus";
    case minus: return "minus";
    case asterisk: return "asterisk";
    case div_sign: return "div_sign";
    case equal_sign: return "equal_sign";
    case colon: return "colon";
    case comma: return "comma";
    case dot: return "dot";
    case kw_const: return "kw_const";
    case kw_else: return "kw_else";
    case kw_fun: return "kw_fun";
    case kw_if: return "kw_if";
    case kw_inside: return "kw_inside";
    case kw_loop: return "kw_loop";
    case kw_return: return "kw_return";
    case kw_struct: return "kw_struct";
    case kw_type: return "kw_type";
    case kw_union: return "kw_union";
    case kw_var: return "kw_var";
  };
}

class LexerSample {
  std::string_view src;
  Lexer lexer;
  size_t lastPos;

 public:
  LexerSample() = default;
  LexerSample(std::string_view src) : src(src) {}

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

  ~LexerSample() {
    auto token = next();
    EXPECT_TRUE(token.kind == Token::Kind::eof)
        << "lexer is not exhausted, next token is " << repr(token);
  }

 private:
  Token next() {
    auto token = lexer.next(src.substr(lexer.getPos()));
    lastPos = token.span.end();
    return token;
  }

  std::string_view examine(Span span) {
    return src.substr(span.start(), span.length());
  }

  std::string repr(Token token) {
    auto value = examine(token);
    std::ostringstream repr;
    repr << "Token { kind: " << kindName(token) << ", ";
    auto isSpace = [](char c) -> bool {
      return c == ' ' || c == '\t' || c == '\n';
    };
    if (std::all_of(value.begin(), value.end(), isSpace))
      repr << "span: [" << token.span.start() << ", " << token.span.end()
           << ")";
    else
      repr << "value: \"" << value << "\"";
    repr << " }";
    return std::move(repr).str();
  }

  AssertionResult failure(Token token) {
    // TODO: deinit this
    return AssertionFailure() << "got " << repr(token);
  }
};

}  // namespace

TEST(LexerTests, Empty) {
  LexerSample totallyEmpty("");
  LexerSample lexicallyEmpty(" \t\n\t\t\n\n");
}

TEST(LexerTests, Numbers) {
  LexerSample numbers("0 7 126 0042");
  ASSERT_TRUE(numbers.nextTokenIs(integer, "0"));
  ASSERT_TRUE(numbers.nextTokenIs(integer, "7"));
  ASSERT_TRUE(numbers.nextTokenIs(integer, "126"));
  ASSERT_TRUE(numbers.nextTokenIs(integer, "0042"));
}

TEST(LexerTests, Words) {
  LexerSample idents("camelCase PascalCase snake_case CAPS withN0m8ers");
  ASSERT_TRUE(idents.nextTokenIs(ident, "camelCase"));
  ASSERT_TRUE(idents.nextTokenIs(ident, "PascalCase"));
  ASSERT_TRUE(idents.nextTokenIs(ident, "snake_case"));
  ASSERT_TRUE(idents.nextTokenIs(ident, "CAPS"));
  ASSERT_TRUE(idents.nextTokenIs(ident, "withN0m8ers"));
  LexerSample keywords(
      "const "
      "else "
      "fun "
      "if "
      "inside "
      "loop "
      "return "
      "struct "
      "type "
      "union "
      "var "
      "\n"
  );
  ASSERT_TRUE(keywords.nextTokenIs(kw_const));
  ASSERT_TRUE(keywords.nextTokenIs(kw_else));
  ASSERT_TRUE(keywords.nextTokenIs(kw_fun));
  ASSERT_TRUE(keywords.nextTokenIs(kw_if));
  ASSERT_TRUE(keywords.nextTokenIs(kw_inside));
  ASSERT_TRUE(keywords.nextTokenIs(kw_loop));
  ASSERT_TRUE(keywords.nextTokenIs(kw_return));
  ASSERT_TRUE(keywords.nextTokenIs(kw_struct));
  ASSERT_TRUE(keywords.nextTokenIs(kw_type));
  ASSERT_TRUE(keywords.nextTokenIs(kw_union));
  ASSERT_TRUE(keywords.nextTokenIs(kw_var));
}

TEST(LexerTests, Regions) {
  LexerSample regions("(  [ {   } ]  )");
  ASSERT_TRUE(regions.nextTokenIs(parens_begin));
  ASSERT_TRUE(regions.nextTokenIs(brackets_begin));
  ASSERT_TRUE(regions.nextTokenIs(scope_begin));
  ASSERT_TRUE(regions.nextTokenIs(scope_end));
  ASSERT_TRUE(regions.nextTokenIs(brackets_end));
  ASSERT_TRUE(regions.nextTokenIs(parens_end));
  LexerSample and_scopes(
      "scope\n"
      "\t{\n"
      "\t  (\n"
      "\t\t\t[\n"
      "\t    \n"
      "\t\t\t]\n"
      "\t  )\n"
      "\t}\n"
  );
  ASSERT_TRUE(and_scopes.nextTokenIs(ident, "scope"));
  ASSERT_TRUE(and_scopes.nextTokenIs(scope_begin));
  ASSERT_TRUE(and_scopes.nextTokenIs(scope_begin));
  ASSERT_TRUE(and_scopes.nextTokenIs(parens_begin));
  ASSERT_TRUE(and_scopes.nextTokenIs(brackets_begin));
  ASSERT_TRUE(and_scopes.nextTokenIs(brackets_end));
  ASSERT_TRUE(and_scopes.nextTokenIs(parens_end));
  ASSERT_TRUE(and_scopes.nextTokenIs(scope_end));
  ASSERT_TRUE(and_scopes.nextTokenIs(scope_end));
}

TEST(LexerTests, Punctuation) {
  LexerSample punct("+-*/=,.");
  ASSERT_TRUE(punct.nextTokenIs(plus));
  ASSERT_TRUE(punct.nextTokenIs(minus));
  ASSERT_TRUE(punct.nextTokenIs(asterisk));
  ASSERT_TRUE(punct.nextTokenIs(div_sign));
  ASSERT_TRUE(punct.nextTokenIs(equal_sign));
  ASSERT_TRUE(punct.nextTokenIs(comma));
  ASSERT_TRUE(punct.nextTokenIs(dot));
}

TEST(LexerTests, Newline) {
  LexerSample newlines(
      "type Example struct\n"
      "  first int;\n"
      "  second bool;\n"
      "\n"
      "var example Example = Example\n"
      "  first: 0,\n"
      "  second: false,\n"
      "\n"
      "type Compact struct { first bool; second int }\n"
  );
  ASSERT_TRUE(newlines.nextTokenIs(kw_type));
  ASSERT_TRUE(newlines.nextTokenIs(ident, "Example"));
  ASSERT_TRUE(newlines.nextTokenIs(kw_struct));
  ASSERT_TRUE(newlines.nextTokenIs(scope_begin));
  ASSERT_TRUE(newlines.nextTokenIs(ident, "first"));
  ASSERT_TRUE(newlines.nextTokenIs(ident, "int"));
  ASSERT_TRUE(newlines.nextTokenIs(newline));
  ASSERT_TRUE(newlines.nextTokenIs(ident, "second"));
  ASSERT_TRUE(newlines.nextTokenIs(ident, "bool"));
  ASSERT_TRUE(newlines.nextTokenIs(newline));
  ASSERT_TRUE(newlines.nextTokenIs(scope_end));

  ASSERT_TRUE(newlines.nextTokenIs(kw_var));
  ASSERT_TRUE(newlines.nextTokenIs(ident, "example"));
  ASSERT_TRUE(newlines.nextTokenIs(ident, "Example"));
  ASSERT_TRUE(newlines.nextTokenIs(equal_sign));
  ASSERT_TRUE(newlines.nextTokenIs(ident, "Example"));
  ASSERT_TRUE(newlines.nextTokenIs(scope_begin));
  ASSERT_TRUE(newlines.nextTokenIs(ident, "first"));
  ASSERT_TRUE(newlines.nextTokenIs(colon));
  ASSERT_TRUE(newlines.nextTokenIs(integer, "0"));
  ASSERT_TRUE(newlines.nextTokenIs(comma));
  ASSERT_TRUE(newlines.nextTokenIs(ident, "second"));
  ASSERT_TRUE(newlines.nextTokenIs(colon));
  ASSERT_TRUE(newlines.nextTokenIs(ident, "false"));
  ASSERT_TRUE(newlines.nextTokenIs(comma));
  ASSERT_TRUE(newlines.nextTokenIs(scope_end));

  ASSERT_TRUE(newlines.nextTokenIs(kw_type));
  ASSERT_TRUE(newlines.nextTokenIs(ident, "Compact"));
  ASSERT_TRUE(newlines.nextTokenIs(kw_struct));
  ASSERT_TRUE(newlines.nextTokenIs(scope_begin));
  ASSERT_TRUE(newlines.nextTokenIs(ident, "first"));
  ASSERT_TRUE(newlines.nextTokenIs(ident, "bool"));
  ASSERT_TRUE(newlines.nextTokenIs(newline));
  ASSERT_TRUE(newlines.nextTokenIs(ident, "second"));
  ASSERT_TRUE(newlines.nextTokenIs(ident, "int"));
  ASSERT_TRUE(newlines.nextTokenIs(scope_end));
}
