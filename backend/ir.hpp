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
  Const,
  Print,
};

enum class VType { Int, Unit, Ref };

VType string2type(string type);
IType string2instr(string instr);

struct Arg {
  VType type;
  Arg(json &value);
  string name;
};

struct Value {
  VType type;
  variant<string, int> val;
};

struct Instr {
  IType op;
  VType type;
  optional<string> dest;
  vector<Value> args;

public:
  Instr(json &instr);
};

class Function {
  string name;
  vector<Arg> args;
  VType type;
  vector<Instr> instrs;

public:
  Function(json &fn);
  friend ostream &operator<<(ostream &o, const Function &fn);
};
