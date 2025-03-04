#include "fn.hpp"

std::ostream &operator<<(std::ostream &stream, const Function &fn) {
  stream << t2s[fn.type] << " ";
  stream << fn.name;
  stream << "(";
  auto sep = "";
  for (auto &it : fn.args) {
    stream << sep << it.name << ": " << t2s[it.type];
    sep = ", ";
  }
  stream << ") {" << std::endl;
  for (auto &block : fn.blocks) {
    stream << block;
  }
  stream << "}";
  return stream;
}
