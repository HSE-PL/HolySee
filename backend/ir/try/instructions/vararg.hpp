#pragma once

#include "instruction.hpp"
#include <cassert>

class Call : public Instruction {
  friend class IFactory;
  std::string fname;

  Call(std::string fname, vptr dest, std::list<vptr> arglist) : fname(fname) {
    assert(dest->type() == ValType::Ref);
    dest_ = dest;
    args = arglist;
  }
};

class Print : public Instruction {
  friend class IFactory;

  Print(std::list<vptr> arglist) {
    dest_ = nullptr;
    args = arglist;
  }
};
