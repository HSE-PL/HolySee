#include "program.hpp"

std::ostream &operator<<(std::ostream &stream, const Program &p) {
  for (auto &fn : p.fns) {
    stream << fn << std::endl;
  }
  return stream;
}
