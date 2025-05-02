#ifndef HSEC_FRONTEND_AST_NODE_H
#define HSEC_FRONTEND_AST_NODE_H

#include "ast/print.h"

namespace hsec::frontend::ast {

struct Node : print::Printable {
  virtual ~Node() {}

 protected:
  friend Printer& operator<<(Printer& printer, const Node& node) {
    node.print(printer);
    return printer;
  }

  virtual void print(Printer&) const = 0;
};

using Ident = std::string;

}  // namespace hsec::frontend::ast

#endif
