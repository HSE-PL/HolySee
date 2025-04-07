#pragma once

#include "instruction.hpp"

class Add final : public Instruction {
  friend class IOStreamer;

public:
  friend class MCtx;
  virtual mach emit(MCtx &ctx) { return ctx.visit(*this); }
  Add(vptr dest, vptr val1, vptr val2);
  Add(const Add &other) = default;
  virtual void accept(IOStreamer &io) { io.visit(*this); }
  virtual ~Add() = default;
};

class Sub final : public Instruction {
  friend class IOStreamer;

public:
  friend class MCtx;
  virtual mach emit(MCtx &ctx) { return ctx.visit(*this); }
  Sub(vptr dest, vptr val1, vptr val2);
  Sub(const Sub &other) = default;
  virtual void accept(IOStreamer &io) { io.visit(*this); }
  virtual ~Sub() = default;
};

class Br : public Instruction {
  friend class IOStreamer;

public:
  friend class MCtx;
  virtual mach emit(MCtx &ctx) { return ctx.visit(*this); }
  Br(vptr cond, vptr l1, vptr l2);
  Br(const Br &other) = default;
  virtual void accept(IOStreamer &io) { io.visit(*this); }
  virtual ~Br() = default;
};

class Eq final : public Instruction {
  friend class IOStreamer;

public:
  friend class MCtx;
  virtual mach emit(MCtx &ctx) { return ctx.visit(*this); }
  Eq(vptr dest, vptr c1, vptr c2);
  Eq(const Eq &other) = default;
  virtual void accept(IOStreamer &io) { io.visit(*this); }
  virtual ~Eq() = default;
};
