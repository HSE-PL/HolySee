#ifndef HSEC_FRONTEND_PARSING_H
#define HSEC_FRONTEND_PARSING_H

#include <optional>
#include <utility>

#include "ast.h"
#include "lexicon.h"

namespace hsec::frontend::parsing {

namespace internal {
template <typename T, typename U>
concept decays_to = std::same_as<std::decay_t<T>, U>;
}

template <typename T>
concept Context = requires(T ctx, Span span) {
  { ctx.next() } -> internal::decays_to<Token>;
  { ctx.peek() } -> internal::decays_to<Token>;
  { ctx.view(span) } -> internal::decays_to<std::string_view>;
};

Token expect(Token::Kind kind, Context auto&& ctx) {
  auto token = ctx.next();
  if (token.kind != kind)
    throw "FORCED";
  return token;
}

template <typename Context, typename Parser>
auto parseSeparated(
    Context&& ctx, Parser&& parser, Token::Kind sep, Token::Kind end
) {
  std::vector<decltype(parser(ctx))> items;
  if (ctx.peek().kind == end)
    return items;
  for (;;) {
    items.push_back(parser(ctx));
    if (ctx.peek().kind == sep)
      ctx.next();
    if (ctx.peek().kind == end)
      break;
  }
  return items;
}

std::shared_ptr<ast::Type> parseType(Context auto&& ctx);

ast::Field parseField(Context auto&& ctx) {
  auto name = ctx.view(expect(Token::Kind::word, ctx));
  auto type = parseType(ctx);
  return ast::Field(std::string(name), type);
}

std::shared_ptr<ast::Type> parseType(Context auto&& ctx) {
  auto name = ctx.view(expect(Token::Kind::word, ctx));
  if (name == "struct") {
    expect(Token::Kind::begin, ctx);
    return std::make_shared<ast::StructType>(parseSeparated(
        ctx,
        [](Context auto&& ctx) { return parseField(ctx); },
        Token::Kind::next,
        Token::Kind::end
    ));
    expect(Token::Kind::end, ctx);
  } else
    return std::make_shared<ast::NamedType>(std::string(name));
}

ast::TypeDecl parseTypeDecl(Context auto&& ctx) {
  if (ctx.view(expect(Token::Kind::word, ctx)) != "type")
    throw "FORCED";
  auto name = ctx.view(expect(Token::Kind::word, ctx));
  auto type = parseType(ctx);
  return ast::TypeDecl(std::string(name), type);
}

}  // namespace hsec::frontend::parsing

#endif
