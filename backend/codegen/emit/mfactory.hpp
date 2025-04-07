#pragma once

#include "../mir/mcontrol.hpp"
#include "../mir/minstrs.hpp"
#include <memory>

using mptr = std::shared_ptr<MachineInstr>;
using uptr = std::shared_ptr<Mach>;

class MFactory {

public:
  virtual mptr createAdd(uptr dest, uptr lhs, uptr rhs) = 0;
  virtual ~MFactory() {}
};

class MFactoryNaive : public MFactory {

public:
  virtual uptr createImm(int val) { return std::make_shared<Imm>(val); }

  virtual mptr createAdd(uptr dest, uptr lhs, uptr rhs) {
    return std::make_shared<MAdd>(dest, lhs, rhs);
  }
  virtual mptr createSub(uptr dest, uptr lhs, uptr rhs) {
    return std::make_shared<MSub>(dest, lhs, rhs);
  }
  virtual mptr createMov(uptr dest, uptr rhs) {
    return std::make_shared<Mov>(dest, rhs, rhs->type());
  }
  virtual mptr createMCmp(uptr lhs, uptr rhs) {
    return std::make_shared<MCmp>(lhs, rhs);
  }
  virtual mptr createPush(uptr rhs) { return std::make_shared<Push>(rhs); }
  virtual mptr createPop(uptr rhs) { return std::make_shared<Pop>(rhs); }
  virtual mptr createMRet() { return std::shared_ptr<MRet>(new MRet()); }
  virtual std::shared_ptr<MLabel> createLabel(std::string name) {
    return std::make_shared<MLabel>(name);
  }
  virtual mptr createMCall(std::shared_ptr<MLabel> label) {
    return std::make_shared<MCall>(label);
  }
  virtual mptr createMJmp(std::shared_ptr<MLabel> label) {
    return std::make_shared<MJmp>(label);
  }
  virtual mptr createMJe(std::shared_ptr<MLabel> label) {
    return std::make_shared<MJe>(label);
  }
  virtual mptr createMJne(std::shared_ptr<MLabel> label) {
    return std::make_shared<MJne>(label);
  }
};
