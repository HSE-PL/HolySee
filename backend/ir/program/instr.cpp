#include "instr.hpp"
#include "../../backend.hpp"

std::ostream &operator<<(std::ostream &stream, const Instr &instr) {
  stream << "  ";
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
  case VType::Func: {
    auto fn = instr.vals.cfncall();
    stream << " " << fn.first;
    for (auto &&arg : fn.second) {
      stream << " " << arg;
    }
  } break;
  case VType::Label: {
    std::string label;
    label = instr.vals.clabel();
    stream << " " << label;
  } break;
  case VType::Refs: {
    auto h = instr.vals.crefp();
    stream << " " << h.first;
    stream << " " << h.second;

  } break;
  case VType::Branches: {
    auto h = instr.vals.cbranches();
    stream << " " << h.ccond();
    stream << " " << h.ctrueb();
    stream << " " << h.cfalseb();
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
  case VType::Bool:
  case VType::Int:
    stream << " " << instr.vals.cnumber();
    break;
  default:
    throw NotImplemented("Instr out" + t2s[instr.vals.type()]);
  }
  stream << std::endl;
  return stream;
}
