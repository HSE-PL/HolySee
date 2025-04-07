#pragma once

#include "instruction.hpp"
#include <cassert>
#include <memory>

class Const : public Instruction {
  friend class IOStreamer;

public:
  friend class MCtx;
  virtual mach emit(MCtx &ctx) { return ctx.visit(*this); }
  Const(vptr dest, vptr val) {
    assert(dest->type() == ValType::Ref);
    assert(val->type() == ValType::Const);
    assert(std::dynamic_pointer_cast<Int>(val));
    dest_ = dest;
    args.push_back(val);
  }
  virtual void accept(IOStreamer &io) { io.visit(*this); }
  virtual ~Const() = default;
};

class Id : public Instruction {
  friend class IOStreamer;

public:
  friend class MCtx;
  virtual mach emit(MCtx &ctx) { return ctx.visit(*this); }
  Id(vptr dest, vptr val) {
    assert(dest->type() == ValType::Ref);
    assert(val->type() == ValType::Ref);
    assert(std::dynamic_pointer_cast<Ref>(val));
    dest_ = dest;
    args.push_back(val);
  }
  virtual void accept(IOStreamer &io) { io.visit(*this); }
  virtual ~Id() = default;
};

class Jmp : public Instruction {
  friend class IOStreamer;

public:
  friend class MCtx;
  virtual mach emit(MCtx &ctx) { return ctx.visit(*this); }
  Jmp(vptr label) {
    assert(label->type() == ValType::Label);
    args.push_back(label);
    // i may or may not think that this is bs later.
    dest_ = nullptr;
    /*dest_ = std::make_shared<Value>(new Unit());*/
  }
  virtual void accept(IOStreamer &io) { io.visit(*this); }
  virtual ~Jmp() = default;
};

class Ret : public Instruction {
  friend class IOStreamer;

public:
  friend class MCtx;
  virtual mach emit(MCtx &ctx) { return ctx.visit(*this); }
  Ret(vptr val) {
    assert(val->type() == ValType::Ref);
    args.push_back(val);
    // i may or may not think that this is bs later.
    dest_ = nullptr;
    /*dest_ = std::make_shared<Value>(new Unit());*/
  }
  virtual void accept(IOStreamer &io) { io.visit(*this); }
  virtual ~Ret() = default;
};
