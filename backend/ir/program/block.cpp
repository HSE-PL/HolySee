#include "block.hpp"

std::ostream &operator<<(std::ostream &stream, const Block &block) {
  stream << block.name << ":" << std::endl;
  for (auto &instr : block.instrs) {
    stream << instr;
  }
  return stream;
}
