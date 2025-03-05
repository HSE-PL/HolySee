#include "pass_manager.hpp"

std::ostream &operator<<(std::ostream &stream, const PassManager &p) {
  stream << p.program_;
  return stream;
}
