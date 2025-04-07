#pragma once

#include "instruction.hpp"
#include <cassert>

class Call : public Instruction {
  friend class IOStreamer;
  std::string fname;

public:
  friend class MCtx;
  virtual mach emit(MCtx &ctx) { return ctx.visit(*this); }
  Call(std::string fname, vptr dest, std::list<vptr> arglist) : fname(fname) {
    assert(dest->type() == ValType::Ref);
    dest_ = dest;
    args = arglist;
  }

  virtual void accept(IOStreamer &io) { io.visit(*this); }
  virtual ~Call() = default;
};

class Print : public Instruction {
  friend class IOStreamer;

public:
  friend class MCtx;
  virtual mach emit(MCtx &ctx) { return ctx.visit(*this); }
  Print(std::list<vptr> arglist) {
    dest_ = nullptr;
    args = arglist;
  }
  virtual void accept(IOStreamer &io) { io.visit(*this); }
  virtual ~Print() = default;
};
