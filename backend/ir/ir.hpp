#pragma once

#include "../json/json.hpp"
#include <ostream>
#include <string>
#include <variant>
#include <vector>

using namespace std;
using namespace nlohmann;

enum class IType {
  Add,
  Sub,
  Const,
  Print,
  Ret,
  Call,
  Jmp,
  Br,
  Eq,
};

enum class VType {
  Int,
  Bool,
  Unit,
  Ref,
  Label,
  Func,
};

VType string2type(string type);
IType string2instr(string instr);

struct Arg {
  VType type;
  Arg(json &value);
  string name;
};

struct Value {
  VType type;
  variant<pair<string, string>, string, int> val;
  Value(VType type, variant<pair<string, string>, string, int> val)
      : type(type), val(val) {}
};

struct Instr {
  IType op;
  VType type;
  optional<string> dest;
  vector<Value> args;

public:
  Instr(json &instr);
};

struct Block {
  string name;
  vector<Instr> instrs;
  Block(string name, vector<Instr> instrs) : name(name), instrs(instrs) {}
  Block(vector<Instr> instrs) : instrs(instrs) {}
  void add_instr(Instr instr) { instrs.push_back(instr); }
};

struct Function {
  string name;
  vector<Arg> args;
  VType type;
  vector<Block> blocks;

  Function(json &fn);
  friend ostream &operator<<(ostream &o, const Function &fn);
};

struct Program {
  vector<Function> fns;
  Program(vector<Function> fns) : fns(fns) {}
  friend ostream &operator<<(ostream &o, const Program &fn);
};

class Pass {
public:
  virtual void pass(Function &fn) = 0;
};

class PassManager {
  Program program;

public:
  PassManager(Program p) : program(p) {}
  void pass(Pass &pass) {
    for (auto &fn : program.fns) {
      pass.pass(fn);
    }
  }
  friend ostream &operator<<(ostream &o, const PassManager &fn);
};
