#include "instr.hpp"
#include "../../backend.hpp"

Instr::Instr(nlohmann::json &instr) {
  op = string2instr(instr["op"]);
  if (instr.contains("type")) {
    type = string2type(instr["type"]);
  } else {
    type = VType::Unit;
  }
  if (instr.contains("dest")) {
    dest = instr["dest"];
  } else {
    dest = std::nullopt;
  }
  assert(instr.contains("funcs") || instr.contains("labels") ||
         instr.contains("value") || instr.contains("args"));
  if (instr.contains("funcs")) {
    args.push_back(Value(VType::Func, (std::string)(instr["funcs"][0])));
  }
  if (instr.contains("labels")) {
    switch (op) {
    case IType::Br:
      args.push_back(Value(VType::Label, (std::string)(instr["labels"][0])));
      args.push_back(Value(VType::Label, (std::string)(instr["labels"][1])));
      break;
    case IType::Jmp:
      args.push_back(Value(VType::Label, (std::string)(instr["labels"][0])));
    default:
      throw NotImplemented(
          "somehow mixed up and labels are in wrong instruction" + i2s[op]);
    }
    args.push_back(Value(VType::Label, (std::string)(instr["labels"][0])));
  }
  if (instr.contains("value")) {
    args.push_back(Value(VType::Int, (int)instr["value"]));
  } else if (instr.contains("args")) {
    for (auto it : instr["args"]) {
      args.push_back(Value(VType::Ref, (std::string)it));
    }
  }
}

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
