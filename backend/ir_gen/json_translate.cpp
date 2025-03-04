#include "json_translate.hpp"
#include "../backend.hpp"
#include "json.hpp"

Arg arg(nlohmann::json &value) {
  Arg arg{};
  arg.type = string2type(value.at("type"));
  arg.name = value.at("name");
  return arg;
}

Instr instr(nlohmann::json &instr) {
  Instr ins{};
  ins.op = string2instr(instr["op"]);
  if (instr.contains("type")) {
    ins.type = string2type(instr["type"]);
  } else {
    ins.type = VType::Unit;
  }
  if (instr.contains("dest")) {
    ins.dest = instr["dest"];
  } else {
    ins.dest = std::nullopt;
  }
  assert(instr.contains("funcs") || instr.contains("labels") ||
         instr.contains("value") || instr.contains("args"));
  if (instr.contains("funcs")) {
    ins.args.push_back(Value(VType::Func, (std::string)(instr["funcs"][0])));
  }
  if (instr.contains("labels")) {
    switch (ins.op) {
    case IType::Br:
      ins.args.push_back(
          Value(VType::Label, (std::string)(instr["labels"][0])));
      ins.args.push_back(
          Value(VType::Label, (std::string)(instr["labels"][1])));
      break;
    case IType::Jmp:
      ins.args.push_back(
          Value(VType::Label, (std::string)(instr["labels"][0])));
    default:
      throw NotImplemented(
          "somehow mixed up and labels are in wrong instruction" + i2s[ins.op]);
    }
    ins.args.push_back(Value(VType::Label, (std::string)(instr["labels"][0])));
  }

  if (instr.contains("value")) {
    ins.args.push_back(Value(VType::Int, (int)instr["value"]));
  } else if (instr.contains("args")) {
    for (auto it : instr["args"]) {
      ins.args.push_back(Value(VType::Ref, (std::string)it));
    }
  }
  return ins;
}

void fn2ir(nlohmann::json &fn, Function &func) {
  assert(s2i.size() == i2s.size() && t2s.size() == s2t.size());
  func.name = fn["name"];
  Block b = Block(func.name, std::vector<Instr>());
  for (auto it : fn["instrs"]) {
    if (it.contains("label")) {
      func.blocks.push_back(b);
      b = Block(it["label"], std::vector<Instr>());
      continue;
    }
    b.add_instr(instr(it));
  }
  func.blocks.push_back(b);
  for (auto it : fn["args"]) {
    func.args.push_back(arg(it));
  }
}

PassManager JsonTranslator::json2pm(std::string &json) {
  nlohmann::json info = nlohmann::json::parse(json);
  auto fns = info["functions"];
  std::vector<Function> functions;
  for (auto fn : fns) {
    Function func{};
    fn2ir(fn, func);
    functions.push_back(func);
  }
  return PassManager(Program(functions));
}
