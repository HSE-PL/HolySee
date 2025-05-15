#include <iostream>
#include <sstream>

#include "lexing.h"
#include "parsing.h"

using namespace hsec::frontend;
using namespace parsing;
using lexing::Indent;

int main() {
  std::ostringstream input;
  input << std::cin.rdbuf();

  StringViewStream stream(input.view());
  std::ostringstream out;
  ast::print::OStreamPrinter printer(
      out, Indent{.style = Indent::Style::spaces, .width = 4}
  );
  for (const auto& decl : parsing::parse(stream))
    printer << *decl;

  std::cout << std::move(out).str() << std::flush;
}
