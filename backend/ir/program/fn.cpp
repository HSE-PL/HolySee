#include "fn.hpp"

Arg::Arg(nlohmann::json &value) {
  type = string2type(value.at("type"));
  name = value.at("name");
}

Function::Function(nlohmann::json &fn) {
  assert(s2i.size() == i2s.size() && t2s.size() == s2t.size());
  name = fn["name"];
  Block b = Block(name, std::vector<Instr>());
  for (auto it : fn["instrs"]) {
    if (it.contains("label")) {
      blocks.push_back(b);
      b = Block(it["label"], std::vector<Instr>());
      continue;
    }
    auto instr = Instr(it);
    b.add_instr(Instr(it));
  }
  blocks.push_back(b);
  for (auto it : fn["args"]) {
    args.push_back(Arg(it));
  }
}

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
