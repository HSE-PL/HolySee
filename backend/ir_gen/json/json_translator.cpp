#include "json_translator.hpp"
#include "../../backend.hpp"
#include "json.hpp"
#include <vector>

Arg arg(nlohmann::json &value) {
  Arg arg{};
  arg.type = string2type(value.at("type"));
  arg.name = value.at("name");
  return arg;
}

void i2json(Instr &instr) {
  nlohmann::json json;
  json["op"] = i2s.at(instr.op);
  json["type"] = t2s.at(instr.type);
  if (instr.dest.has_value()) {
    json["dest"] = *instr.dest;
  }

  /*switch(instr.args*/
  /*switch (instr.op) {*/
  /*case IType::Const:*/
  /*  json["value"] = instr.args[0].val;*/
  /*  break;*/
  /*}*/

  /*nlohmann::json ins = {*/
  /*  "op": */
  /*}*/
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
    ins.vals = Val(VType::Func, (std::string)(instr["funcs"][0]));
  }
  if (instr.contains("labels")) {
    switch (ins.op) {
    case IType::Br:
      ins.vals =
          Val(VType::Branches, std::pair((std::string)instr["labels"][0],
                                         (std::string)instr["labels"][1]));
      break;
    case IType::Jmp:
      ins.vals = Val(VType::Label, (std::string)(instr["labels"][0]));
    default:
      throw NotImplemented(
          "somehow mixed up and labels are in wrong instruction" + i2s[ins.op]);
    }
  }

  if (instr.contains("value")) {
    ins.vals = Val(VType::Int, (int)instr["value"]);
  } else if (instr.contains("args")) {
    switch (ins.op) {
    case IType::Ret:
    case IType::Eq:
    case IType::Br:
      ins.vals = Val(VType::Ref, (std::string)instr["args"][0]);
      break;
    case IType::Print:
    case IType::Call: {
      std::vector<std::string> vec;
      for (auto &&v : instr["args"]) {
        vec.push_back(v);
      }
      ins.vals = Val(VType::VArg, vec);
    } break;
    default:
      ins.vals = Val(VType::Refs, std::pair((std::string)instr["args"][0],
                                            (std::string)instr["args"][1]));
      break;
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

PassManager JsonTranslator::toIR(std::string json) {
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
