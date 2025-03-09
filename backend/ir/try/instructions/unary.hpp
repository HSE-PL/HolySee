#pragma once

#include "instruction.hpp"
#include <cassert>
#include <memory>

class Const : public Instruction {
  friend class IFactory;
  Const(vptr dest, vptr val) {
    assert(dest->type() == ValType::Ref);
    assert(val->type() == ValType::Const);
    assert(std::dynamic_pointer_cast<Int>(val));
    dest_ = dest;
    args.push_back(val);
  }
};

class Jmp : public Instruction {
  friend class IFactory;
  Jmp(vptr label) {
    assert(label->type() == ValType::Label);
    args.push_back(label);
    // i may or may not think that this is bs later.
    dest_ = nullptr;
    /*dest_ = std::make_shared<Value>(new Unit());*/
  }
};

class Ret : public Instruction {
  friend class IFactory;
  Ret(vptr val) {
    assert(val->type() == ValType::Ref);
    args.push_back(val);
    // i may or may not think that this is bs later.
    dest_ = nullptr;
    /*dest_ = std::make_shared<Value>(new Unit());*/
  }
};
