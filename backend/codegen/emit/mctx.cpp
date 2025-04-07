#include "mctx.hpp"

#include "../../ir/blocks/bblock.hpp"
#include "../../ir/fn.hpp"
#include "../../ir/instructions/binary.hpp"
#include "../../ir/instructions/instruction.hpp"
#include "../../ir/instructions/unary.hpp"
#include "../../ir/instructions/vararg.hpp"
#include "../../ir/program.hpp"
#include "../../ir/streamers/iostreamer.hpp"
#include "../../ir/value.hpp"
#include <algorithm>
#include <exception>
#include <iostream>
#include <iterator>
#include <memory>
#include <ostream>

// this file and whole codegen is hardcoded for x86.
// and it is very naive in it's method of codegen.
// no selection dags, no lowering, no bullshit, just straight up
// stupid and naive codegen.
// i don't want to support anything other than 1 specific backend.
// so this either will stay hardcoded as x86, or maybe it will be changed for
// different arch. but i don't want to generalize this logic, so abi and
// registers and all is pretty hardcoded. well, sucks, well, shucks.

GPR RegAlloc::alloc() {
  if (regs.size() == 0) // yeah for now just crash af if there are no scratches
    throw std::exception();
  auto ret = regs[regs.size() - 1];
  regs.pop_back();
  return ret;
}

void RegAlloc::free(GPR gpr) { regs.push_back(gpr); }

GPR MCtx::allocScratch() { return ralloc.alloc(); }

Placement MCtx::putOnStack() { return Placement(Stack, current_stack++); }

std::shared_ptr<MPlaced> MCtx::movPlacementToReg(mach m) {
  auto reg = allocScratch();
  auto newPlacement = Placement(Register, (size_t)reg);
  auto placed = std::make_shared<MPlaced>(newPlacement, m->type());
  auto mov = factory->createMov(placed, std::dynamic_pointer_cast<MValue>(m));
  instrs.push_back(mov);
  return placed;
}

std::shared_ptr<MPlaced> MCtx::movPlacementToStack(std::shared_ptr<MPlaced> m) {
  auto newPlacement = putOnStack();
  auto placed = std::make_shared<MPlaced>(newPlacement, m->type());
  auto mov = factory->createMov(placed, std::dynamic_pointer_cast<MValue>(m));
  instrs.push_back(mov);
  switch (placed->place().type()) {
  case Register:
    ralloc.free((GPR)placed->place().place());
  case Stack:
    break;
  }
  return placed;
}

void MCtx::genMoveImm(std::shared_ptr<MValue> v, int val) {
  auto imm = factory->createImm(val);
  auto mov = factory->createMov(v, imm);
  instrs.push_back(mov);
}

mach MCtx::visit(Int &i32) {
  auto place = putOnStack();
  auto val = std::make_shared<MInt>(place);
  genMoveImm(val, i32.val());
  return val;
}

mach MCtx::visit(Id &id) {
  auto place = putOnStack();
  auto val = std::make_shared<MInt>(place);
  auto reg = ralloc.alloc();
  auto holder =
      std::make_shared<MPlaced>(Placement(Register, (size_t)reg), MType::i32);
  auto ref = std::dynamic_pointer_cast<Ref>(id.args.front());
  auto destRef = std::dynamic_pointer_cast<Ref>(id.dest_);
  auto toCopy = vals.at(ref->name());
  instrs.push_back(factory->createMov(holder, toCopy));
  instrs.push_back(factory->createMov(val, holder));
  vals.insert({destRef->name(), val});
  ralloc.free(reg);
  return val;
}

mach MCtx::visit(Bool &boolean) {
  auto place = putOnStack();
  auto val = std::make_shared<MBool>(place);
  genMoveImm(val, boolean.val());
  return val;
}

mach MCtx::visit(Add &add) {
  assert(add.args.size() == 2);
  auto lhs = add.args.front()->emit(*this);
  auto rhs = add.args.back()->emit(*this);
  auto lhsReg = movPlacementToReg(lhs);
  auto rhsReg = movPlacementToReg(rhs);

  auto ref = std::dynamic_pointer_cast<Ref>(add.dest_);

  auto madd = factory->createAdd(lhsReg, lhsReg, rhsReg);
  instrs.push_back(madd);
  auto res = movPlacementToStack(lhsReg);
  ralloc.free((GPR)lhsReg->place().place());
  ralloc.free((GPR)rhsReg->place().place());
  vals.insert({ref->name(), res});
  return madd;
}

mach MCtx::visit(Sub &sub) {
  assert(sub.args.size() == 2);
  auto lhs = sub.args.front()->emit(*this);
  auto rhs = sub.args.back()->emit(*this);
  auto lhsReg = movPlacementToReg(lhs);
  auto rhsReg = movPlacementToReg(rhs);

  auto ref = std::dynamic_pointer_cast<Ref>(sub.dest_);

  auto madd = factory->createSub(lhsReg, lhsReg, rhsReg);
  instrs.push_back(madd);
  auto res = movPlacementToStack(lhsReg);
  ralloc.free((GPR)lhsReg->place().place());
  ralloc.free((GPR)rhsReg->place().place());
  vals.insert({ref->name(), res});
  return madd;
}

mach MCtx::visit(Const &cnst) {
  assert(cnst.args.size() == 1);
  auto imm = std::dynamic_pointer_cast<MPlaced>(cnst.args.front()->emit(*this));
  auto dest = std::dynamic_pointer_cast<Ref>(cnst.dest_);
  vals.insert({dest->name(), imm});
  return imm;
}

mach MCtx::visit(Ref &ref) { return vals.at(ref.name()); }

mach MCtx::visit(BBlock &block) {
  IOStreamer io(std::cout);
  for (auto &&instr : block.instrs) {
    auto i = instr.get();
    i->accept(io);
    std::cout << "hehe" << std::endl;
    instr->emit(*this);
  }
  std::vector instrVec(instrs.begin(), instrs.end());
  auto b = std::make_shared<MachineBasicBlock>("." + block.name, instrVec);
  bend();
  return b;
}

mach MCtx::visit(Br &br) {
  br.args.pop_front();
  auto tbranch = std::dynamic_pointer_cast<Label>(br.args.front());
  br.args.pop_front();
  auto fbranch = std::dynamic_pointer_cast<Label>(br.args.front());
  auto mtbranch = factory->createLabel(tbranch->label());
  auto mfbranch = factory->createLabel(fbranch->label());
  instrs.push_back(factory->createMJe(mtbranch));
  instrs.push_back(factory->createMJne(mfbranch));
  return mtbranch;
}

mach MCtx::visit(Ret &ret) {
  auto arg = ret.args.front();
  auto place = arg->emit(*this);
  auto newPlace = std::make_shared<MPlaced>(
      Placement(Register, (size_t)GPR::rax), MType::i32);
  auto mov = factory->createMov(newPlace, place);
  instrs.push_back(mov);
  auto ep = genEpilogue();
  for (auto &instr : ep) {
    instrs.push_back(instr);
  }
  epilogue.clear();
  return mov; // ???
}

mach MCtx::visit(Jmp &jmp) {
  auto label = std::dynamic_pointer_cast<Label>(jmp.args.front())->label();
  auto mjmp = factory->createMJmp(factory->createLabel(label));
  instrs.push_back(mjmp);
  return mjmp;
}

mach MCtx::visit(Label &label) { return factory->createLabel(label.label()); }

std::shared_ptr<MachineInstr> MCtx::putArg(GPR reg,
                                           std::shared_ptr<Value> val) {
  auto ref = std::dynamic_pointer_cast<Ref>(val);
  auto oldRegPlacement = Placement(Register, (size_t)reg);
  auto newPlacement = Placement(Stack, current_stack++);
  auto newMValue = std::make_shared<MPlaced>(newPlacement, MType::i32);
  auto oldMValue = std::make_shared<MPlaced>(oldRegPlacement, MType::i32);
  vals.insert({ref->name(), newMValue});
  auto mov = factory->createMov(newMValue, oldMValue);
  ralloc.free(reg);
  return mov;
}

std::list<std::shared_ptr<MachineInstr>> MCtx::genArgs(Fn &fn) {
  std::list<GPR> argsABI = {
      GPR::rdi, GPR::rsi, GPR::rdx, GPR::rcx, GPR::r8, GPR::r9,
  };

  for (auto &v : fn.args) {
    auto reg = argsABI.front();
    argsABI.pop_front();
    auto i = putArg(reg, v);
    prologue.push_back(i);
  }

  return prologue;
}

std::list<std::shared_ptr<MachineInstr>> MCtx::genPrologue(Fn &fn) {
  auto newRbpPlacement = Placement(Register, (int)GPR::rbp);
  auto newRspPlacement = Placement(Register, (int)GPR::rsp);
  auto rbpPlaced =
      std::make_shared<MPlaced>(newRbpPlacement, MType::i32); // hardcoded a bit
  auto rspPlaced =
      std::make_shared<MPlaced>(newRspPlacement, MType::i32); // hardcoded a bit
  prologue.push_front(factory->createSub(
      rspPlaced, rspPlaced, std::make_shared<Imm>(current_stack * 8)));
  prologue.push_front(factory->createMov(rbpPlaced, rspPlaced));
  prologue.push_front(factory->createPush(rbpPlaced));

  // now args.

  return prologue;
}

std::list<std::shared_ptr<MachineInstr>> MCtx::genEpilogue() {
  auto newRbpPlacement = Placement(Register, (int)GPR::rbp);
  auto newRspPlacement = Placement(Register, (int)GPR::rsp);
  auto rbpPlaced =
      std::make_shared<MPlaced>(newRbpPlacement, MType::i32); // hardcoded a bit
  auto rspPlaced =
      std::make_shared<MPlaced>(newRspPlacement, MType::i32); // hardcoded a bit
  epilogue.push_back(factory->createMov(rspPlaced, rbpPlaced));
  epilogue.push_back(factory->createPop(rbpPlaced));
  epilogue.push_back(factory->createMRet());
  return epilogue;
}

mach MCtx::visit(Call &call) {
  std::list<GPR> argsABI = {
      GPR::rdi, GPR::rsi, GPR::rdx, GPR::rcx, GPR::r8, GPR::r9,
  };
  for (auto &&arg : call.args) {
    auto place = arg->emit(*this);
    auto reg = argsABI.front();
    auto newPlacement = Placement(Register, (size_t)reg);
    argsABI.pop_front();
    auto newArgPlace = std::make_shared<MPlaced>(newPlacement, MType::i32);
    instrs.push_back(factory->createMov(newArgPlace, place));
  }
  auto label = factory->createLabel(call.fname);
  auto mcall = factory->createMCall(label);
  instrs.push_back(mcall);
  if (call.type() != ValType::Nothing) {
    auto destRef = std::dynamic_pointer_cast<Ref>(call.dest_);
    auto retReg = std::make_shared<MPlaced>(
        Placement(Register, (size_t)GPR::rax), MType::i32);
    auto newPlace = std::make_shared<MPlaced>(putOnStack(), MType::i32);
    instrs.push_back(factory->createMov(newPlace, retReg));
    vals.insert({destRef->name(), newPlace});
  }
  return mcall;
}

mach MCtx::visit(Eq &eq) {
  auto lhs = std::dynamic_pointer_cast<Ref>(eq.args.front());
  auto rhs = std::dynamic_pointer_cast<Ref>(eq.args.back());
  auto lhsPlace = vals[lhs->name()];
  auto rhsPlace = vals[rhs->name()];
  auto regLhs = ralloc.alloc();
  auto regRhs = ralloc.alloc();
  auto newPlaceLhs = std::make_shared<MPlaced>(
      Placement(Register, (size_t)regLhs), MType::boolean);
  auto newPlaceRhs = std::make_shared<MPlaced>(
      Placement(Register, (size_t)regRhs), MType::boolean);
  instrs.push_back(factory->createMov(newPlaceLhs, lhsPlace));
  instrs.push_back(factory->createMov(newPlaceRhs, rhsPlace));
  instrs.push_back(factory->createMCmp(newPlaceLhs, newPlaceRhs));
  ralloc.free(regLhs);
  ralloc.free(regRhs);
  return newPlaceRhs;
}

mach MCtx::visit(Fn &fn) {
  auto func = std::make_shared<MachineFunction>(fn.name());
  genArgs(fn);
  for (auto &&block : fn.blocks) {
    func->addBlock(
        std::dynamic_pointer_cast<MachineBasicBlock>(block->emit(*this)));
  }
  if (fn.type() == Type::Unit) {
    genEpilogue();
  }
  auto prologue = genPrologue(fn);
  func->addPrologue(prologue);
  func->addEpilogue(epilogue);
  fend();
  return func;
}

mach MCtx::visit(Program &program) {
  auto exec = std::make_shared<MachineExec>();
  for (auto &&[name, fn] : program.funcs) {
    exec->addFunc(std::dynamic_pointer_cast<MachineFunction>(fn->emit(*this)));
  }
  return exec;
}
