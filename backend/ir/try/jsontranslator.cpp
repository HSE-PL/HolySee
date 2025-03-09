#include "blocks/bbuilder.hpp"
#include "fnbuilder.hpp"
#include "program.hpp"
#include "json/json.hpp"
#include <functional>
#include <unordered_map>

using namespace nlohmann;

// this is very funny unoptimized piece of junk
//
// better than last attempt at this tho.

using std::function;
using std::unordered_map, std::string, std::vector, std::pair;
using signature_map = unordered_map<string, pair<Type, vector<Type>>>;

unordered_map<std::string, Type> s2t = {
    {"int", Type::Int},
    {"bool", Type::Bool},
};

class JTranslationCtx {
  signature_map signatures;
  string current_func;

  void buildSignatures(json &fns) {
    for (auto &fn : fns) {
      assert(fn.contains("args"));
      assert(fn.contains("name"));
      assert(fn.contains("type"));
      auto name = fn.at("name");
      auto fntype = fn.at("type");
      vector<Type> types;
      for (auto &arg : fn.at("args")) {
        assert(arg.contains("type"));
        auto type = arg.at("type");
        assert(s2t.contains(type));
        types.push_back(s2t.at(type));
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
  auto type = instr.at("type");
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
void j2br(BBuilder &b, json &instr, JTranslationCtx &ctx) {}
void j2print(BBuilder &b, json &instr, JTranslationCtx &ctx) {}
void j2jmp(BBuilder &b, json &instr, JTranslationCtx &ctx) {}
void j2const(BBuilder &b, json &instr, JTranslationCtx &ctx) {}

using fn = std::function<void(BBuilder &, json &, signature_map &)>;

unordered_map<string,
              function<void(BBuilder &, json &instr, JTranslationCtx &)>>
    s2i = {{"add", j2add},     {"sub", j2sub}, {"call", j2call},
           {"eq", j2eq},       {"ret", j2ret}, {"br", j2br},
           {"print", j2print}, {"jmp", j2jmp}, {"const", j2const}};

void parse_instr(BBuilder &builder, json &instr, JTranslationCtx &ctx) {
  assert(instr.contains("op"));
  /*switch()*/
}

void func(json &fn, JTranslationCtx &ctx) {
  assert(fn.contains("name"));
  auto name = fn.at("name");
  ctx.setFuncName(name);
  Type t(Type::Unit);
  if (fn.contains("type")) {
    t = s2t.at(fn.at("type"));
  }
  FnBuilder builder(t, name);
  BBuilder block_builder(name);
  for (auto &it : fn.at("instrs")) {
    if (it.contains("label")) {
      block_builder.reset(it.at("label"));
      continue;
    }
    parse_instr(block_builder, it, ctx);
  }
}

void toIR(std::string jsonstr) {
  auto code = json::parse(jsonstr);
  auto fns = code.at("functions");
  Program p;
  for (auto &fn : fns) {
    // addFunc;
  }
}
