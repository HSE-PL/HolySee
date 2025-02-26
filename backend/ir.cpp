#include "ir.hpp"
#include "backend.hpp"

#include <ostream>
#include <unordered_map>

using namespace std;
using namespace nlohmann;

unordered_map<string, VType> typeMap = {
    {"int", VType::Int},
    //
};

unordered_map<string, IType> instrMap = {
    {"const", IType::Const}, {"add", IType::Add}, {"print", IType::Print}};

VType string2type(string type) {
  if (!typeMap.contains(type)) {
    throw NotImplemented();
  }
  return typeMap.at(type);
}

IType string2instr(string instr) {
  if (!instrMap.contains(instr)) {
    throw NotImplemented();
  }
  return instrMap.at(instr);
}

Value::Value(json &value) {
  type = string2type(value.at("type"));
  name = value["name"];
}

Instr::Instr(json &instr) {
  op = string2instr(instr["op"]);
  type = string2type(instr["type"]);
  dest = instr["dest"];
  if (instr.contains("value")) {
    args.push_back(instr["value"]);
  } else if (instr.contains("args")) {
    for (auto it : instr["args"]) {
      args.push_back(it);
    }
  } else {
    throw NotImplemented();
  }
}

Function::Function(json &fn) {
  name = fn["name"];
  for (auto it : fn["instrs"]) {
    instrs.push_back(Instr(it));
  }
  for (auto it : fn["args"]) {
    args.push_back(Value(it));
  }
}
