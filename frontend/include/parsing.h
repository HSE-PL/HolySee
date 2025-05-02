#ifndef HSEC_FRONTEND_PARSING_H
#define HSEC_FRONTEND_PARSING_H

#include <vector>

#include "ast.h"
#include "token.h"

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

template <typename T, typename U>
concept Parser = Context<U> && requires(T&& parser, U&& ctx) { parser(ctx); };

Token expect(Context auto&& ctx, Token::Kind kind) {
  auto token = ctx.next();
  if (token.kind != kind)
    throw "TODO";
  return token;
}

auto parseSeparated(
    Context auto&& ctx,
    Parser<decltype(ctx)> auto&& parser,
    Token::Kind sep,
    Token::Kind end
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

ast::Ident parseIdent(Context auto&& ctx) {
  return ast::Ident(ctx.view(expect(ctx, Token::ident)));
}

ast::Type parseType(Context auto&& ctx);
ast::Field parseField(Context auto&& ctx) {
  auto name = ctx.view(expect(ctx, Token::Kind::ident));
  auto type = parseType(ctx);
  return ast::Field(std::string(name), type);
}

ast::Type parseType(Context auto&& ctx) {
  using enum Token::Kind;

  switch (auto token = ctx.next(); token.kind) {
    case ident:
      return std::make_shared<ast::NamedType>(std::string(ctx.view(token)));
    case kw_struct: {
      expect(ctx, Token::Kind::begin);
      auto struct_type = std::make_shared<ast::StructType>(parseSeparated(
          ctx, parseField<decltype(ctx)>, Token::Kind::next, Token::Kind::end
      ));
      expect(ctx, Token::Kind::end);
      return struct_type;
    }
    case kw_union: {
      expect(ctx, Token::Kind::begin);
      auto struct_type = std::make_shared<ast::UnionType>(parseSeparated(
          ctx, parseField<decltype(ctx)>, Token::Kind::next, Token::Kind::end
      ));
      expect(ctx, Token::Kind::end);
      return struct_type;
    }
    default:
      throw "TODO";
  }
}

ast::Decl parseDecl(Context auto&& ctx) {
  using enum Token::Kind;
  switch (auto token = ctx.next(); token.kind) {
    case kw_type: {
      auto name = parseIdent(ctx);
      auto type = parseType(ctx);
      return std::make_shared<ast::TypeDecl>(name, type);
    }
    default:
      throw "TODO";
  }
}

std::vector<ast::Decl> parse(Context auto&& ctx) {
  std::vector<ast::Decl> decls;
  while (ctx.peek().kind != Token::eof)
    decls.push_back(parseDecl(ctx));
  return decls;
}

}  // namespace hsec::frontend::parsing

#endif
