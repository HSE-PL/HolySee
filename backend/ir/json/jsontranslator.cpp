#include "../blocks/bbuilder.hpp"
#include "../fnbuilder.hpp"
#include "../program.hpp"
#include "json.hpp"
#include <functional>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include <iostream>

using namespace nlohmann;

// this is very funny unoptimized piece of junk
//
// better than last attempt at this tho.

using std::function;
using std::unordered_map, std::string, std::vector, std::pair;
using signature_map = unordered_map<string, pair<Type, vector<Type>>>;

unordered_map<std::string, Type> s2tt = {
    {"int", Type::Int},
    {"bool", Type::Bool},
};

class JTranslationCtx {
  signature_map signatures;
  string current_func;

  void buildSignatures(json &fns) {
    for (auto &fn : fns) {
      assert(fn.contains("name"));
      auto name = fn.at("name");
      auto fntype = Type::Unit;
      if (fn.contains("type")) {
        fntype = s2tt.at(fn.at("type"));
      }
      vector<Type> types;
      if (!fn.contains("args")) {
        signatures.insert({(string)name, {fntype, types}});
        continue;
      }
      for (auto &arg : fn.at("args")) {
        assert(arg.contains("type"));
        auto type = arg.at("type");
        assert(s2tt.contains(type));
        types.push_back(s2tt.at(type));
      }
      signatures.insert({name, {fntype, types}});
    }
  }

public:
  const vector<Type> &getArgTypes(std::string fname) const {
    return signatures.at(fname).second;
  }
  Type fnType(std::string fname) const { return signatures.at(fname).first; }
  void setFuncName(std::string name) { current_func = name; }
  const std::string &getCurrentFuncName() const { return current_func; }
  JTranslationCtx(json &fns) { buildSignatures(fns); }
};

// a lil bit of code duplication but w/e
void j2add(BBuilder &b, json &instr, JTranslationCtx &ctx) {
  assert(instr.contains("dest"));
  auto dest = instr.at("dest");
  auto arg1 = instr.at("args").at(0);
  auto arg2 = instr.at("args").at(1);
  b.createAdd(dest, arg1, arg2);
}
void j2sub(BBuilder &b, json &instr, JTranslationCtx &ctx) {
  assert(instr.contains("dest"));
  auto dest = instr.at("dest");
  auto arg1 = instr.at("args").at(0);
  auto arg2 = instr.at("args").at(1);
  b.createSub(dest, arg1, arg2);
}

void j2eq(BBuilder &b, json &instr, JTranslationCtx &ctx) {
  assert(instr.contains("dest"));
  auto dest = instr.at("dest");
  auto arg1 = instr.at("args").at(0);
  auto arg2 = instr.at("args").at(1);
  b.createEq(dest, arg1, arg2);
}

void j2call(BBuilder &b, json &instr, JTranslationCtx &ctx) {
  assert(instr.contains("funcs"));
  assert(instr.contains("dest"));
  assert(instr.contains("args"));
  auto fncall = instr.at("funcs").at(0);
  auto dest = instr.at("dest");
  auto type = s2tt.at(instr.at("type"));
  auto &&types = ctx.getArgTypes(fncall).cbegin();
  assert(ctx.getArgTypes(fncall).size() == instr.at("args").size());
  std::list<std::pair<Type, string>> arglist;
  for (auto &&arg : instr.at("args")) {
    arglist.push_back({*types, arg});
  }
  b.createCall(fncall, type, dest, arglist);
}
void j2ret(BBuilder &b, json &instr, JTranslationCtx &ctx) {
  assert(instr.contains("args"));
  auto ret_type = ctx.fnType(ctx.getCurrentFuncName());
  b.createRet(ret_type, instr.at("args").at(0));
}
void j2br(BBuilder &b, json &instr, JTranslationCtx &ctx) {
  assert(instr.contains("labels"));
  assert(instr.contains("args"));
  auto cond = instr.at("args").at(0);
  auto l1 = instr.at("labels").at(0);
  auto l2 = instr.at("labels").at(1);
  b.createBr(cond, l1, l2);
}

// for now i default print's args to be int
// i mean in ideal world i don't have print at all
// but it will have to wait till 2nd milestone i suppose.
void j2print(BBuilder &b, json &instr, JTranslationCtx &ctx) {
  assert(instr.contains("args"));
  std::list<pair<Type, string>> arglist;
  for (auto &&arg : instr.at("args")) {
    arglist.push_back({Type::Int, arg});
  }
  b.createPrint(arglist);
}

void j2jmp(BBuilder &b, json &instr, JTranslationCtx &ctx) {
  assert(false && "no jmps yet");
}

void j2id(BBuilder &b, json &instr, JTranslationCtx &ctx) {
  b.createId(instr.at("dest"), instr.at("args").at(0));
}

void j2const(BBuilder &b, json &instr, JTranslationCtx &ctx) {
  assert(instr.contains("dest"));
  assert(instr.contains("value"));
  assert(instr.contains("type"));
  // hardcode int and everything for now, for now don't care about any
  // other posssibilities.
  b.createConstInt(instr.at("dest"), (int)instr.at("value"));
}

using fn = std::function<void(BBuilder &, json &, signature_map &)>;

unordered_map<string,
              function<void(BBuilder &, json &instr, JTranslationCtx &)>>
    s2ii = {{"add", j2add},     {"sub", j2sub}, {"call", j2call},
            {"eq", j2eq},       {"ret", j2ret}, {"br", j2br},
            {"print", j2print}, {"jmp", j2jmp}, {"const", j2const},
            {"id", j2id}};

std::unordered_set<string> terminators = {"ret", "br", "jmp"};

void parse_instr(BBuilder &builder, json &instr, JTranslationCtx &ctx) {
  assert(instr.contains("op"));
  s2ii.at((string)instr.at("op"))(builder, instr, ctx);
}

std::shared_ptr<Fn> func(json &fn, JTranslationCtx &ctx) {
  assert(fn.contains("name"));
  string name = fn.at("name");
  ctx.setFuncName(name);
  Type t(Type::Unit);
  if (fn.contains("type")) {
    t = s2tt.at(fn.at("type"));
  }
  FnBuilder builder(t, name);
  if (fn.contains("args")) {
    for (auto &&arg : fn.at("args")) {
      auto argtype = s2tt.at(arg.at("type"));
      auto argname = arg.at("name");
      builder.addArg(argtype, argname);
    }
  }
  BBuilder block_builder(name);
  for (auto &it : fn.at("instrs")) {
    if (it.contains("label")) {
      block_builder.reset(it.at("label"));
      continue;
    }
    parse_instr(block_builder, it, ctx);
    if (terminators.contains(it.at("op"))) {
      builder.addBlock(block_builder.build());
    }
  }
  if (!fn.contains("type")) {
    builder.addBlock(block_builder.build());
  }
  return builder.build();
}

Program toIR(std::string &jsonstr) {
  auto code = json::parse(jsonstr);
  auto fns = code.at("functions");
  JTranslationCtx ctx(fns);
  Program p;
  for (auto &fn : fns) {
    auto lol = func(fn, ctx);
    p.addFunc(lol);
  }
  return p;
}
