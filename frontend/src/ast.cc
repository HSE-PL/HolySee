#include "ast.h"

namespace hsec::frontend::ast {

std::ostream& TypeDecl::print(std::ostream& out) const {
  out << "type" << ' ' << name << ' ' << *type;
  return out;
}

std::ostream& NamedType::print(std::ostream& out) const {
  out << name;
  return out;
}

std::ostream& Field::print(std::ostream& out) const {
  out << name << ' ' << *type;
  return out;
}

std::ostream& StructType::print(std::ostream& out) const {
  out << "struct" << ' ';
  out << "{\n";
  for (const auto& field : fields)
    out << field << ";\n";
  out << "}";
  return out;
}

}  // namespace hsec::frontend::ast
