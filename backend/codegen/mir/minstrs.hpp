#pragma once

#include "./mcontrol.hpp"
#include "mvalue.hpp"
#include <memory>

using vptr = std::shared_ptr<MValue>;
using uptr = std::shared_ptr<Mach>;

class MCall : public MachineInstr {
  std::shared_ptr<MLabel> fn;

public:
  virtual std::string emit() { return "call " + fn->label(); }
  MCall(std::shared_ptr<MLabel> fn) : MachineInstr(MType::label), fn(fn) {}
};

class MAdd : public MachineInstr {
  uptr dest;
  uptr lhs;
  uptr rhs;

public:
  virtual std::string emit() {
    return "add " + lhs->emit() + ", " + rhs->emit();
  }
  MAdd(uptr dest, uptr lhs, uptr rhs)
      : MachineInstr(MType::i32), dest(dest), lhs(lhs), rhs(rhs) {}
};

class MSub : public MachineInstr {
  uptr dest;
  uptr lhs;
  uptr rhs;

public:
  virtual std::string emit() {
    return "sub " + lhs->emit() + ", " + rhs->emit();
  }
  MSub(uptr dest, uptr lhs, uptr rhs)
      : MachineInstr(MType::i32), dest(dest), lhs(lhs), rhs(rhs) {}
};

class Mov : public MachineInstr {
  uptr dest;
  uptr rhs;

public:
  virtual std::string emit() {
    return "mov " + dest->emit() + ", " + rhs->emit();
  }
  Mov(uptr dest, uptr rhs, MType tp) : MachineInstr(tp), dest(dest), rhs(rhs) {}
};

class Push : public MachineInstr {
  uptr rhs;

public:
  virtual std::string emit() { return "push " + rhs->emit(); }
  Push(uptr rhs) : MachineInstr(MType::i32), rhs(rhs) {}
};

class Pop : public MachineInstr {
  uptr rhs;

public:
  virtual std::string emit() { return "pop " + rhs->emit(); }
  Pop(uptr rhs) : MachineInstr(MType::i32), rhs(rhs) {}
};

class MRet : public MachineInstr {
public:
  virtual std::string emit() { return "ret"; }
  MRet() : MachineInstr(MType::i32) {}
};

class MCmp : public MachineInstr {
  uptr lhs;
  uptr rhs;

public:
  virtual std::string emit() {
    return "cmp " + lhs->emit() + ", " + rhs->emit();
  }
  MCmp(uptr lhs, uptr rhs) : MachineInstr(MType::i32), lhs(lhs), rhs(rhs) {}
};

class MJmp : public MachineInstr {
  std::shared_ptr<MLabel> label;

public:
  virtual std::string emit() { return "jmp " + label->emit(); }
  MJmp(std::shared_ptr<MLabel> label)
      : MachineInstr(MType::label), label(label) {}
};

class MJe : public MachineInstr {
  std::shared_ptr<MLabel> label;

public:
  virtual std::string emit() { return "je ." + label->label(); }
  MJe(std::shared_ptr<MLabel> label)
      : MachineInstr(MType::label), label(label) {}
};

class MJne : public MachineInstr {
  std::shared_ptr<MLabel> label;

public:
  virtual std::string emit() { return "jne ." + label->label(); }
  MJne(std::shared_ptr<MLabel> label)
      : MachineInstr(MType::label), label(label) {}
};
