#pragma once

#include "bblock.hpp"
#include "ifactory.hpp"
#include "vfactory.hpp"

class BBuilder {
  BBlock *block;

public:
  BBuilder(std::string name) { block = new BBlock(name); }
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
  BBuilder &createCall(std::string fname, Type fntype, std::string dest,
                       std::list<std::pair<Type, std::string>> args) {
    std::list<std::shared_ptr<Value>> arglist;
    for (auto &&[t, str] : args) {
      arglist.push_back(VFactory::createRef(t, str));
    }
    std::shared_ptr<Value> pdest = VFactory::createRef(fntype, dest);

    block->addInstr(IFactory::createCall(fname, pdest, arglist));

    return *this;
  }
};
