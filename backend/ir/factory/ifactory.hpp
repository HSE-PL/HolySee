#pragma once

#include "../instructions/binary.hpp"
#include "../instructions/unary.hpp"
#include "../instructions/vararg.hpp"
#include <memory>

// it's not exactly *traditional* factory, but it serves same purposes.
//
// NOTE: whole reason for this class' existence is that at some
// point in  the future i want to offload creating new dest's to here.
class IFactory {
  using vptr = std::shared_ptr<Value>;
  using iptr = std::shared_ptr<Instruction>;

public:
  static iptr createAdd(vptr dest, vptr arg1, vptr arg2) {
    return std::make_shared<Add>(dest, arg1, arg2);
  }
  static iptr createSub(vptr dest, vptr arg1, vptr arg2) {
    return std::make_shared<Sub>(dest, arg1, arg2);
  }
  static iptr createConst(vptr dest, vptr val) {
    return std::make_shared<Const>(dest, val);
  }
  static iptr createJmp(vptr label) { return std::make_shared<Jmp>(label); }
  static iptr createRet(vptr val) { return std::make_shared<Ret>(val); }
  static iptr createCall(std::string name, vptr dest, std::list<vptr> arglist) {
    return std::make_shared<Call>(name, dest, arglist);
  }
  static iptr createBr(vptr cond, vptr l1, vptr l2) {
    return std::make_shared<Br>(cond, l1, l2);
  }
  static iptr createPrint(std::list<vptr> args) {
    return std::make_shared<Print>(args);
  }
  static iptr createEq(vptr dest, vptr c1, vptr c2) {
    return std::make_shared<Eq>(dest, c1, c2);
  }
};
