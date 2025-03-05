#include "json_translator.hpp"
#include "../../backend.hpp"
#include "json.hpp"
#include <vector>

static Arg arg(nlohmann::json &value) {
  Arg arg{};
  arg.type = string2type(value.at("type"));
  arg.name = value.at("name");
  return arg;
}
static nlohmann::json i2json(Instr &instr) {
  nlohmann::json json;
  json["op"] = i2s.at(instr.op);
  if (instr.type != VType::Unit) {
    json["type"] = t2s.at(instr.type);
  }
  if (instr.dest.has_value()) {
    json["dest"] = *instr.dest;
  }
  switch (instr.vals.type()) {
  case VType::Int:
    json["value"] = instr.vals.cnumber();
    break;
  case VType::Refs: {
    auto h = instr.vals.refp();
    json["args"] = nlohmann::json::array({h.first, h.second});
  } break;
  case VType::Ref:
    json["args"] = {instr.vals.ref()};
    break;
  case VType::Func: {
    auto fn = instr.vals.fncall();
    json["funcs"] = nlohmann::json::array({fn.first});
    json["args"] = fn.second;
  } break;
  case VType::Unit:
    break;
  case VType::Bool:
    throw NotImplemented("Tried to write bool value.");
    break;
  case VType::VArg:
    json["args"] = instr.vals.args();
    break;
  case VType::Label:
    json["labels"] = {instr.vals.label()};
    break;
  case VType::Branches: {
    auto h = instr.vals.branches();
    json["args"] = nlohmann::json::array({h.cond()});
    json["labels"] = {h.trueb(), h.falseb()};
  } break;
  };
  return json;
}

static nlohmann::json fn2json(Function &fn) {
  auto instr_json = nlohmann::json::array();
  for (auto &&block : fn.blocks) {
    auto label = nlohmann::json::object();
    label["label"] = block.name;
    instr_json.push_back(label);
    for (auto &&instr : block.instrs) {
      instr_json.push_back(i2json(instr));
    }
  }
  nlohmann::json fnc;
  fnc["name"] = fn.name;
  if (fn.type != VType::Unit) {
    fnc["type"] = t2s[fn.type];
  }
  fnc["instrs"] = instr_json;
  auto args_json = nlohmann::json::array();
  for (auto &&arg : fn.args) {
    args_json.push_back({{"name", arg.name}, {"type", t2s[arg.type]}});
  }
  fnc["args"] = args_json;

  return fnc;
}

static nlohmann::json p2json(Program &p) {
  auto fns_json = nlohmann::json::array();
  for (auto &&fn : p.fns) {
    fns_json.push_back(fn2json(fn));
  }

  nlohmann::json program_json;
  program_json["functions"] = fns_json;
  return program_json;
}

static Instr instr(nlohmann::json &instr) {
  Instr ins{};
  assert(instr.contains("op"));
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
  // TBH this part needs a complete redo, but im zaebalsya uzhe ot etogo
  // TODO: redo this whole part
  if (instr.contains("labels")) {
    switch (ins.op) {
    case IType::Br: {

      auto arg = (std::string)instr["args"][0];
      ins.vals = Val(VType::Branches,
                     BranchArgs(std::tuple(arg, (std::string)instr["labels"][0],
                                           (std::string)instr["labels"][1])));
    } break;
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
    case IType::Eq: {
      ins.vals = Val(VType::Refs, std::pair((std::string)instr["args"][0],
                                            (std::string)instr["args"][1]));

    } break;
    case IType::Ret:
      ins.vals = Val(VType::Ref, (std::string)instr["args"][0]);
      break;
    case IType::Br:
      break;
    case IType::Print: {
      std::vector<std::string> vec;
      for (auto &&v : instr["args"]) {
        vec.push_back(v);
      }
      ins.vals = Val(VType::VArg, vec);
    } break;
    case IType::Call: {
      auto funcname = instr["funcs"][0];
      std::vector<std::string> vec;
      for (auto &&v : instr["args"]) {
        vec.push_back(v);
      }
      ins.vals = Val(VType::Func, std::pair(funcname, vec));
    } break;
    default:
      ins.vals = Val(VType::Refs, std::pair((std::string)instr["args"][0],
                                            (std::string)instr["args"][1]));
      break;
    }
  }
  return ins;
}

static void fn2ir(nlohmann::json &fn, Function &func) {
  assert(s2i.size() == i2s.size() && t2s.size() == s2t.size());
  func.name = fn["name"];
  if (fn.contains("type")) {
    func.type = s2t[fn["type"]];
  } else {
    func.type = VType::Unit;
  }
  Block b = Block(func.name, std::list<Instr>());
  for (auto it : fn["instrs"]) {
    if (it.contains("label")) {
      func.blocks.push_back(b);
      b = Block(it["label"], std::list<Instr>());
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

std::ostream &JsonTranslator::toStream(std::ostream &stream,
                                       const PassManager &pm) {
  auto prog = pm.program_;
  auto json = p2json(prog);
  stream << json;
  return stream;
}
