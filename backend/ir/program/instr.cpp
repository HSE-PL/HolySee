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
  for (auto &iterargs : instr.args) {
    switch (iterargs.val.index()) {
    case 0:
      stream << " " << get<0>(iterargs.val).first;
      stream << " " << get<0>(iterargs.val).second;
      break;
    case 1:
      stream << " " << get<1>(iterargs.val);
      break;
    case 2:
      stream << " " << get<2>(iterargs.val);
    }
  }
  stream << std::endl;
  return stream;
}
