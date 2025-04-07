#pragma once

#include <ostream>
class Add;
class Sub;
class Br;
class Eq;
class Const;
class Jmp;
class Ret;
class Call;
class Print;
class Ref;
class Label;
class Int;
class Bool;
struct Unit;
class Value;
class Instruction;
class Program;
class Fn;
class BBlock;
class Id;

class IOStreamer {
  std::ostream &stream;
  void args(Instruction &instr);
  void dest(Instruction &instr);
  void parameters(Fn &func);

public:
  IOStreamer(std::ostream &stream) : stream(stream) {}
  void visit(Program &p);
  void visit(BBlock &b);
  void visit(Fn &fn);
  void visit(Add &add);
  void visit(Sub &sub);
  void visit(Br &br);
  void visit(Eq &eq);
  void visit(Const &cnst);
  void visit(Id &id);
  void visit(Jmp &jmp);
  void visit(Ret &ret);
  void visit(Call &call);
  void visit(Print &print);
  void visit(Ref &ref);
  void visit(Label &label);
  void visit(Int &integer);
  void visit(Bool &boolean);
  void visit(Unit &unit);
};
