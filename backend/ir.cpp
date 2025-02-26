#include "ir.hpp"
#include "backend.hpp"

#include <optional>
#include <ostream>
#include <unordered_map>

using namespace std;
using namespace nlohmann;

unordered_map<VType, string> t2s = {
    {VType::Int, "int"},
    //
};

unordered_map<IType, string> i2s = {
    {IType::Const, "const"},
    {IType::Add, "add"},
    {IType::Print, "print"},
};

unordered_map<string, VType> s2t = {
    {"int", VType::Int},
    //
};

unordered_map<string, IType> s2i = {
    {"const", IType::Const},
    {"add", IType::Add},
    {"print", IType::Print},
};

VType string2type(string type) {
  if (!s2t.contains(type)) {
    throw NotImplemented();
  }
  return s2t.at(type);
}

IType string2instr(string instr) {
  if (!s2i.contains(instr)) {
    throw NotImplemented();
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
  // TODO: make this work
  /*if (instr.contains("value")) {*/
  /*  args.push_back(instr["value"]);*/
  /*} else if (instr.contains("args")) {*/
  /*  for (auto it : instr["args"]) {*/
  /*    args.push_back(it);*/
  /*  }*/
  /*} else {*/
  /*  throw NotImplemented();*/
  /*}*/
}

Function::Function(json &fn) {
  name = fn["name"];
  for (auto it : fn["instrs"]) {
    instrs.push_back(Instr(it));
  }
  for (auto it : fn["args"]) {
    args.push_back(Arg(it));
  }
}

ostream &operator<<(ostream &stream, const Function &fn) {
  stream << t2s[fn.type] << " ";
  stream << fn.name;
  stream << "(";
  for (auto &it : fn.args) {
    stream << it.name << ": " << t2s[it.type];
  }
  stream << ") {" << endl;
  for (auto &it : fn.instrs) {
    stream << i2s[it.op] << " : " << t2s[it.type] << " ";
    if (it.dest.has_value()) {
      stream << *it.dest;
    }
    for (auto &iterargs : it.args) {
      switch (iterargs.val.index()) {
      case 0:
        stream << " " << get<0>(iterargs.val);
        break;
      case 1:
        stream << " " << get<1>(iterargs.val);
      }
    }
    stream << endl;
  }
  return stream;
}
