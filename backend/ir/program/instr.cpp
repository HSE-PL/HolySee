#include "instr.hpp"
#include "../../backend.hpp"

std::ostream &operator<<(std::ostream &stream, const Instr &instr) {
  if (instr.dest.has_value()) {
    stream << *instr.dest;
    stream << ": " << t2s[instr.type];
    stream << " = ";
  } else {
    assert(instr.type == VType::Unit);
    stream << "_: unit = ";
  }
  stream << i2s[instr.op];
  switch (instr.vals.type()) {
  case VType::Func:
  case VType::Label: {
    std::string label;
    label = instr.vals.clabel();
    stream << " " << label;
  } break;
  case VType::Refs:
  case VType::Branches: {
    std::pair<std::string, std::string> h;
    h = instr.vals.cbranches();
    stream << " " << h.first;
    stream << " " << h.second;
  } break;
  case VType::Ref: {
    auto h = instr.vals.cref();
    stream << " " << h;
  } break;
  case VType::VArg: {
    auto h = instr.vals.cargs();
    for (auto &&v : h) {
      stream << " " << v;
    }
  } break;
  default:
    stream << " " << instr.vals.cnumber();
  }
  stream << std::endl;
  return stream;
}
