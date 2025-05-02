#ifndef HSEC_FRONTEND_AST_PRINT_H
#define HSEC_FRONTEND_AST_PRINT_H

#include <ostream>
#include <string_view>

#include "lexing.h"
#include "token.h"

namespace hsec::frontend::ast {

struct Printer {
  friend class Scope;
  friend class SingleLine;

  virtual Printer& operator<<(std::string_view) = 0;
  virtual Printer& operator<<(Token::Kind) = 0;
};

}  // namespace hsec::frontend::ast

namespace hsec::frontend::ast::print {

struct Printable {
  friend struct Printer;
};

class Scope {
  Printer& printer;
  Scope(Printer& printer);
  Scope(const Scope&) = delete;
  Scope(Scope&&) = delete;
  ~Scope();
};

}  // namespace hsec::frontend::ast::print

namespace hsec::frontend::ast::print {

class OStreamPrinter : public Printer {
  std::ostream& out;
  lexing::Indent indent;
  bool newline = true;

 public:
  OStreamPrinter(std::ostream& out, lexing::Indent indent)
      : out(out), indent(indent.pattern()) {};
  OStreamPrinter(const OStreamPrinter&) = delete;

  ~OStreamPrinter() { flush(); }

  virtual Printer& operator<<(std::string_view) override;
  virtual Printer& operator<<(Token::Kind) override;

  void flush();
};

}  // namespace hsec::frontend::ast::print

#endif
