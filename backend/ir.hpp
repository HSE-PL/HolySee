#pragma once

#include "json.hpp"
#include <ostream>
#include <string>
#include <vector>

using namespace std;
using namespace nlohmann;

enum class IType {
  Add,
  Const,
  Print,
};

enum class VType { Int };

VType string2type(string type);
IType string2instr(string instr);

struct Value {
  VType type;
  string name;
  Value(json &value);
};

class Instr {
  IType op;
  VType type;
  string dest;
  vector<string> args;

public:
  Instr(json &instr);
};

class Function {
  string name;
  vector<Value> args;
  VType type;
  vector<Instr> instrs;

public:
  Function(json &fn);
  friend ostream &operator<<(ostream &o, const Function &fn);
};
