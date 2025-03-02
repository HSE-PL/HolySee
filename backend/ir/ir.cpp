#include "ir.hpp"
#include "../backend.hpp"
#include "../json/json.hpp"

#include <optional>
#include <ostream>
#include <unordered_map>

using namespace std;
using namespace nlohmann;

unordered_map<VType, string> t2s = {
    {VType::Int, "int"},
    {VType::Unit, "unit"},
    {VType::Ref, "var"},
    {VType::Bool, "bool"},
    //
};

unordered_map<IType, string> i2s = {
    {IType::Const, "const"}, {IType::Add, "add"},   {IType::Print, "print"},
    {IType::Ret, "ret"},     {IType::Call, "call"}, {IType::Br, "br"},
    {IType::Jmp, "j"},       {IType::Eq, "eq"},     {IType::Sub, "sub"},
};

unordered_map<string, VType> s2t = {
    {"int", VType::Int}, {"bool", VType::Bool},
    //
};

unordered_map<string, IType> s2i = {
    {"const", IType::Const}, {"add", IType::Add},   {"print", IType::Print},
    {"ret", IType::Ret},     {"call", IType::Call}, {"br", IType::Br},
    {"jmp", IType::Jmp},     {"eq", IType::Eq},     {"sub", IType::Sub},
};

VType string2type(string type) {
  if (!s2t.contains(type)) {
    throw NotImplemented(" such type doesn't exist " + type);
  }
  return s2t.at(type);
}

IType string2instr(string instr) {
  if (!s2i.contains(instr)) {
    throw NotImplemented("such instruction doesn't exist" + instr);
  }
  return s2i.at(instr);
}

Arg::Arg(json &value) {
  type = string2type(value.at("type"));
  name = value.at("name");
}

Instr::Instr(json &instr) {
  op = string2instr(instr["op"]);
  if (instr.contains("type")) {
    type = string2type(instr["type"]);
  } else {
    type = VType::Unit;
  }
  if (instr.contains("dest")) {
    dest = instr["dest"];
  } else {
    dest = nullopt;
  }
  assert(instr.contains("funcs") || instr.contains("labels") ||
         instr.contains("value") || instr.contains("args"));
  if (instr.contains("funcs")) {
    args.push_back(Value(VType::Func, (string)(instr["funcs"][0])));
  }
  if (instr.contains("labels")) {
    switch (op) {
    case IType::Br:
      args.push_back(Value(VType::Label, (string)(instr["labels"][0])));
      args.push_back(Value(VType::Label, (string)(instr["labels"][1])));
      break;
    case IType::Jmp:
      args.push_back(Value(VType::Label, (string)(instr["labels"][0])));
    default:
      throw NotImplemented(
          "somehow mixed up and labels are in wrong instruction" + i2s[op]);
    }
    args.push_back(Value(VType::Label, (string)(instr["labels"][0])));
  }
  if (instr.contains("value")) {
    args.push_back(Value(VType::Int, (int)instr["value"]));
  } else if (instr.contains("args")) {
    for (auto it : instr["args"]) {
      args.push_back(Value(VType::Ref, (string)it));
    }
  }
}

Function::Function(json &fn) {
  name = fn["name"];
  Block b = Block(name, vector<Instr>());
  for (auto it : fn["instrs"]) {
    if (it.contains("label")) {
      blocks.push_back(b);
      b = Block(it["label"], vector<Instr>());
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

ostream &operator<<(ostream &stream, const Program &p) {
  for (auto &fn : p.fns) {
    stream << fn << endl;
  }
  return stream;
}

ostream &operator<<(ostream &stream, const PassManager &p) {
  stream << p.program;
  return stream;
}

// TODO: abstract part out of here and to same method for instr
ostream &operator<<(ostream &stream, const Function &fn) {
  stream << t2s[fn.type] << " ";
  stream << fn.name;
  stream << "(";
  auto sep = "";
  for (auto &it : fn.args) {
    stream << sep << it.name << ": " << t2s[it.type];
    sep = ", ";
  }
  stream << ") {" << endl;
  for (auto &block : fn.blocks) {
    stream << block.name << ":" << endl;

    for (auto &it : block.instrs) {
      if (it.dest.has_value()) {
        stream << *it.dest;
        stream << ": " << t2s[it.type];
        stream << " = ";
      } else {
        assert(it.type == VType::Unit);
        stream << "_: unit = ";
      }
      stream << i2s[it.op];
      for (auto &iterargs : it.args) {
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
      stream << endl;
    }
  }
  return stream;
}
