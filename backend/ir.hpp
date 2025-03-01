#pragma once

#include "json.hpp"
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
  void add_instr(Instr instr) { instrs.push_back(instr); }
};

class Function {
  string name;
  vector<Arg> args;
  VType type;
  vector<Block> blocks;

public:
  Function(json &fn);
  friend ostream &operator<<(ostream &o, const Function &fn);
};
