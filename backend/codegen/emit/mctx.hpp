#pragma once

// should be generalized more. for now it's okay.

#include "mfactory.hpp"
#include <cassert>
#include <list>
#include <memory>
#include <unordered_map>

// it's not exactly register allocator in usual sense.
// but it allocates registers. so let it be regalloc
class RegAlloc {
  std::vector<GPR> regs = {
      GPR::rax,
      GPR::r10,
      GPR::r11,
  };

public:
  GPR alloc();
  void free(GPR gpr);
};

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

using mach = std::shared_ptr<Mach>;
using valMap = std::unordered_map<std::string, std::shared_ptr<MPlaced>>;

class MCtx {
  std::list<std::shared_ptr<MachineInstr>> prologue;
  std::list<std::shared_ptr<MachineInstr>> epilogue;
  std::list<std::shared_ptr<MachineInstr>> genArgs(Fn &fn);
  std::shared_ptr<MFactoryNaive> factory;
  std::list<std::shared_ptr<MachineInstr>> instrs;
  RegAlloc ralloc;
  size_t current_stack;
  Placement putOnStack();
  void genMoveImm(std::shared_ptr<MValue> v, int val);
  std::list<std::shared_ptr<MachineInstr>> genPrologue(Fn &fn);
  std::list<std::shared_ptr<MachineInstr>> genEpilogue();
  std::shared_ptr<MachineInstr> putArg(GPR reg, std::shared_ptr<Value> val);
  GPR allocScratch();
  std::shared_ptr<MPlaced> movPlacementToReg(mach m);
  std::shared_ptr<MPlaced> movPlacementToStack(std::shared_ptr<MPlaced> m);
  valMap vals;

public:
  MCtx(std::shared_ptr<MFactoryNaive> factory)
      : factory(factory), current_stack(1) {}
  void bend() { instrs.clear(); }
  void fend() {
    current_stack = 1;
    epilogue.clear();
    prologue.clear();
  }
  mach visit(Add &add);
  mach visit(Sub &sub);
  mach visit(Br &br);
  mach visit(Eq &eq);
  mach visit(Const &cnst);
  mach visit(Jmp &jmp);
  mach visit(Call &call);
  mach visit(Ret &ret);
  mach visit(Print &print) {
    assert(false && "well, i don't wanna any prints");
  }
  mach visit(Ref &ref);
  mach visit(Label &label);
  mach visit(Int &i32);
  mach visit(Id &id);
  mach visit(Bool &boolean);
  mach visit(Unit &unit) {
    std::unique_ptr<Mach> p = nullptr;
    return p;
    //
  } // ????
  mach visit(Program &program);
  mach visit(Fn &fn);
  mach visit(BBlock &block);
};
