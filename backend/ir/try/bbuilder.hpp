#pragma once

#include "bblock.hpp"
#include "ifactory.hpp"
#include "vfactory.hpp"

class BBuilder {
  BBlock *block;

public:
  BBuilder(std::string name) { block = new BBlock(name); }
  BBlock build() { return *block; }
  BBuilder &reset(std::string name) {
    delete block;
    block = new BBlock(name);
    return *this;
  }
  BBuilder &setName(std::string name) {
    block->name = name;
    return *this;
  }
  BBuilder &createAdd(std::string dest, std::string a1, std::string a2) {
    auto pdest = VFactory::createIntRef(dest);
    auto parg1 = VFactory::createIntRef(a1);
    auto parg2 = VFactory::createIntRef(a2);
    block->addInstr(IFactory::createAdd(pdest, parg1, parg2));
    return *this;
  }
  BBuilder &createSub(std::string dest, std::string a1, std::string a2) {
    auto pdest = VFactory::createIntRef(dest);
    auto parg1 = VFactory::createIntRef(a1);
    auto parg2 = VFactory::createIntRef(a2);
    block->addInstr(IFactory::createSub(pdest, parg1, parg2));
    return *this;
  }
  BBuilder &createJmp(std::string label) {
    auto l1 = VFactory::createLabel(label);
    block->addInstr(IFactory::createJmp(l1));
    return *this;
  }
  BBuilder &createConstInt(std::string dest, int cnst) {
    auto d1 = VFactory::createIntRef(dest);
    auto operand = VFactory::createInt(cnst);
    block->addInstr(IFactory::createConst(d1, operand));
    return *this;
  }
  BBuilder &createRetInt(std::string ret) {
    auto operand = VFactory::createIntRef(ret);
    block->addInstr(IFactory::createRet(operand));

    return *this;
  }
  BBuilder &createBr(std::string cond, std::string l1, std::string l2) {
    auto pcond = VFactory::createBoolRef(cond);
    auto pl1 = VFactory::createLabel(l1);
    auto pl2 = VFactory::createLabel(l2);
    block->addInstr(IFactory::createBr(pcond, pl1, pl2));
    return *this;
  }
  BBuilder &createCall(std::string fname, Type fntype, std::string dest,
                       std::list<std::pair<Type, std::string>> args) {
    std::list<std::shared_ptr<Value>> arglist;
    for (auto &&[t, str] : args) {
      arglist.push_back(VFactory::createRef(t, str));
    }
    auto pdest = VFactory::createRef(fntype, dest);

    block->addInstr(IFactory::createCall(fname, pdest, arglist));

    return *this;
  }
  BBuilder &createPrint(std::list<std::pair<Type, std::string>> args) {
    std::list<std::shared_ptr<Value>> arglist;
    for (auto &&[t, str] : args) {
      arglist.push_back(VFactory::createRef(t, str));
    }
    block->addInstr(IFactory::createPrint(arglist));
    return *this;
  }
};
