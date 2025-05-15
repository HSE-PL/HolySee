#include "ast.h"

using enum hsec::frontend::Token::Kind;

namespace hsec::frontend::ast::print {

Scope::Scope(Printer& printer) : printer(printer) { printer << begin; }

Scope::~Scope() { printer << end; }

namespace {

using lexing::Indent;

std::ostream& operator<<(std::ostream& out, Indent indent) {
  for (size_t i = 0; i < indent.length(); i++)
    out << (char)indent.style;
  return out;
}

}  // namespace

Printer& OStreamPrinter::operator<<(std::string_view raw) {
  if (newline) {
    if (indent || first)
      out << indent;
    else
      out << "\n";
  } else
    out << ' ';
  out << raw;
  newline = false;
  first = false;
  return *this;
}

Printer& OStreamPrinter::operator<<(Token::Kind token) {
  auto& printer = *this;
  switch (token) {
    case begin:
      out << '\n';
      newline = true;
      ++indent;
      break;
    case end:
      out << '\n';
      newline = true;
      --indent;
      break;
    case next:
      if (!ignoreNext) {
        out << '\n';
        newline = true;
      }
      break;
    case kw_const:
      printer << "const";
      break;
    case kw_fun:
      printer << "fun";
      break;
    case kw_if:
      printer << "if";
      break;
    case kw_inside:
      printer << "inside";
      break;
    case kw_loop:
      printer << "loop";
      break;
    case kw_return:
      printer << "return";
      break;
    case kw_struct:
      printer << "struct";
      break;
    case kw_type:
      printer << "type";
      break;
    case kw_union:
      printer << "union";
      break;
    case kw_var:
      printer << "var";
      break;
    default:
      throw "TODO";
  }
  if (token != next)
    ignoreNext = false;
  if (token == end)
    ignoreNext = true;
  return *this;
}

void OStreamPrinter::flush() { out << std::flush; }

}  // namespace hsec::frontend::ast::print

namespace hsec::frontend::ast {

void Field::print(Printer& printer) const { printer << name << *type; }

void NamedType::print(Printer& printer) const { printer << name; }

void StructType::print(Printer& printer) const {
  printer << kw_struct; 
  for (int i = 0; const auto& field : fields) {
    if (i++)
      printer << next;
    printer << field;
  }
}

void UnionType::print(Printer& printer) const {
  printer << kw_union; 
  print::Scope scope(printer);
  for (int i = 0; const auto& field : fields) {
    if (i++)
      printer << next;
    printer << field;
  }
}

void TypeDecl::print(Printer& printer) const {
  printer << kw_type << name << *type << next;
}
}  // namespace hsec::frontend::ast
